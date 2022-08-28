/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "logger.h"
#include "timer.h"
#include "verbositygroups.h"

#include <array>
#include <atomic>
#include <bit>
#include <cstdio>
#include <cstring>
#include <memory>
#include <semaphore>
#include <thread>
#include <vector>

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
   ~TextLogger(void);

   YM_NO_COPY  (TextLogger)
   YM_NO_ASSIGN(TextLogger)

   bool isOpen(void) const;

   bool open_appendTimeStamp(str const Filename);
   bool open(str const Filename);
   void close(void);

   static constexpr auto getMaxMessageSize_bytes(void) { return _s_MaxMessageSize_bytes; }
   static constexpr auto getMaxNMessagesInBuffer(void) { return _s_MaxNMessagesInBuffer; }
   static constexpr auto getBufferSize_bytes    (void) { return _s_BufferSize_bytes;     }

   void enable(VGGroup_T const VGGroup);
   void enable(VGMask_T  const VGMask );

   void disable(VGGroup_T const VGGroup);
   void disable(VGMask_T  const VGMask );

   template <typename... Args_T>
   void printf(uint32 const    VerbosityGroup,
               str    const    Format,
               Args_T const... Args);

private:
   template <typename... Args_T>
   void printf_Helper(str    const    Format,
                      Args_T const... Args);

   void writeMessagesToFile(void);

   void populateFormattedTime(char * const write_Ptr) const;

   // For description of magic # 34 see populateFormattedTime()
   static constexpr auto getTimeStampSize_bytes(void) { return 34u; }

   static constexpr uint32 _s_MaxMessageSize_bytes = 256u;
   static constexpr uint32 _s_MaxNMessagesInBuffer = 64u;
   static constexpr uint32 _s_BufferSize_bytes     = _s_MaxMessageSize_bytes * _s_MaxNMessagesInBuffer;

   static_assert(std::has_single_bit(_s_MaxNMessagesInBuffer),
      "_s_MaxNMessagesInBuffer needs to be power of 2");

   enum WriterMode_T : uint32
   {
      Closed,
      PreparingToClose,
      Closing,
      Open
   };

   typedef std::counting_semaphore<_s_MaxNMessagesInBuffer>                         MsgSemaphore_T;
   typedef std::array<std::atomic<uint8>, static_cast<uint32>(VGGroup_T::NVGroups)> VGroups_T;

   VGroups_T                 _vGroups;
   std::thread               _writer;
   char * const              _buffer_Ptr;
   Timer                     _timer;
   MsgSemaphore_T            _availableSem;
   MsgSemaphore_T            _messagesSem;
   std::atomic<uint32>       _readPos;  // these positions are slot numbers [0.._s_MaxNMessagesInBuffer)
   std::atomic<uint32>       _writePos; //  :
   std::atomic<WriterMode_T> _writerMode;
   TimeStampMode_T const     _TimeStampMode;
};

/**
 * Using a universal reference here would not be better (ie Args_T && ...), since
 * the arguments are going to be primitives.
 */
template <typename... Args_T>
void TextLogger::printf(uint32 const    VerbosityGroup,
                        str    const    Format,
                        Args_T const... Args)
{
   if (isOpen())
   { // ok to print
      bool isEnabled = false;

      {
         std::scoped_lock const Lock(_verbosityGroups);
         isEnabled = (_verbosityGroups.at(VGroupEnable.Slot) & VGroupEnable.Mask) > 0u;
      }

      if (isEnabled)
      { // verbose enough to print this message
         printf_Helper(Format, Args...);
      }
   }
}

/**
 *
 */
template <typename... Args_T>
void TextLogger::printf_Helper(str    const    Format,
                               Args_T const... Args)
{
   _availableSem.acquire();

   // _writePos doesn't need to wrap, so just incrementing until it rolls over is ok
   auto const WritePos         = _writePos.fetch_add(1u, std::memory_order_acquire) % getMaxNMessagesInBuffer();
   str        writePtr         = _buffer_Ptr + (WritePos * getMaxMessageSize_bytes());
   auto       maxMsgSize_bytes = getMaxMessageSize_bytes();

   if (_TimeStampMode == RecordTimeStamp)
   {
      writePtr += getTimeStampSize_bytes();

      // +1 to account for newline
      static_assert(getMaxMessageSize_bytes() > getTimeStampSize_bytes() + 1u,
         "No room for time stamp plus newline");
      maxMsgSize_bytes -= getTimeStampSize_bytes() + 1u;
   }

   // snprintf writes a null terminator for us
   auto const NCharsWrittenInTheory = std::snprintf(writePtr,
                                                    maxMsgSize_bytes,
                                                    Format,
                                                    Args...);

   if (NCharsWrittenInTheory < 0)
   { // snprintf hit an internal error
      printfError("std::snprintf failed with error code %d! "
                  "Message was '%s'\n",
                  NCharsWrittenInTheory,
                  writePtr);

      std::strncpy(writePtr, "error in printf (internal snprintf error)", getMaxMessageSize_bytes());

      // don't fail here - just keep going
   }
   else if (static_cast<uint32>(NCharsWrittenInTheory) >= maxMsgSize_bytes)
   { // not everything was printed to the buffer
      printfError("Failed to write everything to the buffer! NCharsWrittenInTheory = %ld. "
                  "Msg size = %lu bytes. Message = '%s'\n",
                  NCharsWrittenInTheory,
                  maxMsgSize_bytes,
                  writePtr);

      if (_TimeStampMode == RecordTimeStamp)
      {
         writePtr[maxMsgSize_bytes - 2u] = '\n';
         // last index already null
      }

      // don't fail here - just keep going
   }
   else if (_TimeStampMode == RecordTimeStamp)
   {
      writePtr[NCharsWrittenInTheory     ] = '\n';
      writePtr[NCharsWrittenInTheory + 1u] = '\0';
   }

   // _readPos doesn't need to wrap, so just incrementing until it rolls over is ok
   _readPos.fetch_add(1u, std::memory_order_release);

   _messagesSem.release();
}

} // ym
