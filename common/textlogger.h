/**
 * @file    textlogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "logger.h"
#include "timer.h"
#include "verbositygroup.h"

#include <array>
#include <atomic>
#include <bit>
#include <cstdarg>
#include <semaphore>
#include <thread>

namespace ym
{

/*
 * Convenience functions.
 * -------------------------------------------------------------------------- */

template <Loggable_T... Args_T>
void ymLog(VGMask const    VG,
           str    const    Format,
           Args_T const... Args);

/* -------------------------------------------------------------------------- */

/** TextLogger
 *
 * @brief Logs text to the given outfile - similary to std::printf.
 *
 * @note Uses the multi-producer/single-consumer model.
 */
class TextLogger : public Logger
{
public:
   /** TimeStampMode_T
    *
    * @brief Flag to indicate whether to time stamp the logged line or not.
    */
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

   bool open(str const Filename);
   void close(void);

   static constexpr auto getMaxMessageSize_bytes(void) { return _s_MaxMessageSize_bytes; }
   static constexpr auto getMaxNMessagesInBuffer(void) { return _s_MaxNMessagesInBuffer; }
   static constexpr auto getBufferSize_bytes    (void) { return _s_BufferSize_bytes;     }

   void enable (VGM_T const VG);
   void disable(VGM_T const VG);

   template <Loggable_T... Args_T>
   inline void printf(VGM_T  const    VG,
                      str    const    Format,
                      Args_T const... Args);

private:
   void printf_Handler(VGM_T const  VG,
                       str   const  Format,
                       /*variadic*/ ...);

   void printf_Producer(str const    Format,
                        std::va_list args);

   void writeMessagesToFile(void);

   uint64 populateFormattedTime(char * const write_Ptr) const;

   static constexpr auto _s_MaxMessageSize_bytes = 256_u32;
   static constexpr auto _s_MaxNMessagesInBuffer = 64_u32;
   static constexpr auto _s_BufferSize_bytes     = _s_MaxMessageSize_bytes * _s_MaxNMessagesInBuffer;

   static_assert(std::has_single_bit(_s_MaxNMessagesInBuffer),
                 "_s_MaxNMessagesInBuffer needs to be power of 2");

   /** WriterMode_T
    *
    * @brief State of the logger.
    *
    * @note We differentiate between PreparingToClose and Closing as a way to properly
    *       flush and stop the consumer thread. Communication between the client,
    *       consumer thread, and producer thread requires at least this many states
    *       in this implementation.
    */
   enum WriterMode_T : uint32
   {
      Closed,
      PreparingToClose,
      Closing,
      Open
   };

   using MsgSemaphore_T = std::counting_semaphore<_s_MaxNMessagesInBuffer>;
   using VGroups_T      = std::array<std::atomic<uint8>, VerbosityGroup::getNGroups()>;

   char                      _buffer[_s_BufferSize_bytes];
   VGroups_T                 _vGroups;
   std::thread               _writer;
   Timer                     _timer;
   MsgSemaphore_T            _availableSem;
   MsgSemaphore_T            _messagesSem;
   std::atomic<uint32>       _readPos;  // these positions are slot numbers [0.._s_MaxNMessagesInBuffer)
   std::atomic<uint32>       _writePos; //  :
   std::atomic<WriterMode_T> _writerMode;
   TimeStampMode_T const     _TimeStampMode;
};

/** printf
 * 
 * @brief Print function that constrains argument types.
 * 
 * @tparam Args_T -- Constrained argument types.
 * 
 * @param VG     -- Verbosity level.
 * @param Format -- Format string.
 * @param Args   -- Arguments.
 */
template <Loggable_T... Args_T>
inline void TextLogger::printf(VGM_T  const    VG,
                               str    const    Format,
                               Args_T const... Args)
{
   printf_Handler(VG, Format, Args...);
}

} // ym
