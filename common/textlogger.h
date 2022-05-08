/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "logger.h"
#include "timer.h"

#include <atomic>
#include <cstdio>
#include <semaphore>
#include <thread>

namespace ym
{

/**
 * Uses the multi-producer/single-consumer model.
 */
class TextLogger : public Logger
{
public:
   enum TimeStampMode_T : uint32
   {
      RecordTimeStamp,
      NoRecordTimeStamp
   };

   explicit TextLogger(void);
   explicit TextLogger(TimeStampMode_T const TimeStampMode);
   virtual ~TextLogger(void);

   YM_NO_COPY  (TextLogger)
   YM_NO_ASSIGN(TextLogger)

   virtual bool open(str const Filename) override;
   virtual void close(void) override;

   uint32 getVerbosityCap(void) const;
   void   setVerbosityCap(uint32 const VerbosityCap);

   static constexpr auto getMaxMessageSize_bytes(void) { return _s_MaxMessageSize_bytes; }
   static constexpr auto getMaxNMessagesInBuffer(void) { return _s_MaxNMessagesInBuffer; }
   static constexpr auto getBufferSize_bytes    (void) { return _s_BufferSize_bytes;     }

   template <typename... Args_T>
   bool printf(uint32 const    Verbosity,
               str    const    Format,
               Args_T const... Args);

private:
   template <typename... Args_T>
   void printf_Helper(str    const    Format,
                      Args_T const... Args);

   template <typename... Args_T>
   inline void printfError(str    const    Format,
                           Args_T const... Args);

   void writeMessagesToFile(void);

   void populateFormattedTime(char * const write_Ptr) const;

   static constexpr auto getTimeStampSize_bytes(void) { return 34u; }

   typedef uint64 MsgReadyBF_T; // BF = BitField

   static constexpr uint32 _s_MaxMessageSize_bytes = 256u;
   static constexpr uint32 _s_MaxNMessagesInBuffer = sizeof(MsgReadyBF_T) * 8ul; // 8 bits per byte
   static constexpr uint32 _s_BufferSize_bytes     = _s_MaxMessageSize_bytes * _s_MaxNMessagesInBuffer;

   static_assert((_s_MaxNMessagesInBuffer & (_s_MaxNMessagesInBuffer - 1u)) == 0u,
      "_s_MaxNMessagesInBuffer needs to be power of 2");

   typedef std::counting_semaphore<_s_MaxNMessagesInBuffer> Semaphore_T;

   char                                             _buffer[_s_BufferSize_bytes];
   std::thread  /* --------------------------- */   _writer;
   std::atomic<MsgReadyBF_T>                        _msgReady_bf;
   std::counting_semaphore<_s_MaxNMessagesInBuffer> _availableSem;
   std::counting_semaphore<_s_MaxNMessagesInBuffer> _messagesSem;
   uint32                                           _readPos;  // these positions are slot numbers [0.._s_MaxNMessagesInBuffer)
   std::atomic<uint32> /* ---------------------- */ _writePos; //  :
   std::atomic<uint32>                              _verbosityCap;
   TimeStampMode_T const /* -------------------- */ _TimeStampMode;
   std::atomic_flag                                 _writerStarted;
};

/**
 * Using a universal reference here would not be better (ie Args_T && ...), since
 * the arguments are going to be primitives.
 */
template <typename... Args_T>
bool TextLogger::printf(uint32 const    Verbosity,
                        str    const    Format,
                        Args_T const... Args)
{
   bool wasPrinted = false;

   if (isOpen())
   { // ok to print
      if (Verbosity <= getVerbosityCap())
      { // verbose enough to print this message
         printf_Helper(Format, Args...);
         wasPrinted = true;
      }
   }
   
   return wasPrinted;
}

/**
 *
 */
template <typename... Args_T>
inline void TextLogger::printfError(str    const    Format,
                                    Args_T const... Args)
{
   std::fprintf(stderr, Format, Args...);
}

/**
 * TODO what if the consumer thread isn't running yet? Theoretically possible,
 *      _messagesSem.release() would post to on one.
 * 
 * TODO record time stamp stuff
 */
template <typename... Args_T>
void TextLogger::printf_Helper(str    const    Format,
                               Args_T const... Args)
{
   _availableSem.acquire();

   static_assert(getMaxMessageSize_bytes() > getTimeStampSize_bytes(), "No room for time stamp");
   auto const AdjustedMsgSize_bytes = getMaxMessageSize_bytes() - getTimeStampSize_bytes();
   
   auto const WritePos = _writePos.fetch_add(1, std::memory_order_acquire) % getMaxNMessagesInBuffer();
   str  const WritePtr = _buffer + (WritePos * getMaxMessageSize_bytes()) + getTimeStampSize_bytes();

   // snprintf writes a null terminator for us
   auto const NCharsWrittenInTheory = std::snprintf(WritePtr,
                                                    AdjustedMsgSize_bytes,
                                                    Format,
                                                    Args...);

   if (NCharsWrittenInTheory < 0)
   { // snprintf hit an internal error
      printfError("std::snprintf failed with error code %d! "
                  "Message was '%s'\n",
                  NCharsWrittenInTheory,
                  WritePtr);

      // don't fail here - just keep going
   }
   else if (static_cast<uint32>(NCharsWrittenInTheory) >= AdjustedMsgSize_bytes)
   { // not everything was printed to the buffer
      printfError("Failed to write everything to the buffer! NCharsWrittenInTheory = %ld. "
                  "Max message size = %lu (adjusted = %lu). Message = '%s'\n",
                  NCharsWrittenInTheory,
                  getMaxMessageSize_bytes(),
                  AdjustedMsgSize_bytes,
                  WritePtr);

      // don't fail here - just keep going
   }

   _msgReady_bf.fetch_or(1ul << WritePos, std::memory_order_release);

   _messagesSem.release();
}

} // ym