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
#include "ymception.h"

#include <array>
#include <atomic>
#include <bit>
#include <cstdarg>
#include <mutex>
#include <semaphore>
#include <thread>

namespace ym
{

/*
 * Convenience functions.
 * -------------------------------------------------------------------------- */

template <Loggable... Args_T>
inline void ymLog(VGM_T  const    VG,
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
   enum class TimeStampMode_T : uint32
   {
      RecordTimeStamp,
      NoRecordTimeStamp
   };

   explicit TextLogger(void);
   explicit TextLogger(TimeStampMode_T const TimeStampMode);
   ~TextLogger(void);

   YM_NoCopy  (TextLogger)
   YM_NoAssign(TextLogger)

   YM_DeclYmcept(TextLoggerError_GlobalFailureToOpen)

   static TextLogger * getGlobalInstance(void);

   bool isOpen(void) const;

   bool openToStdout(void);
   bool open(str    const Filename);
   bool open(str    const Filename,
             TSFM_T const TSFilenameMode);
   void close(void);

   static constexpr auto getMaxMessageSize_bytes(void) { return _s_MaxMessageSize_bytes; }
   static constexpr auto getMaxNMessagesInBuffer(void) { return _s_MaxNMessagesInBuffer; }
   static constexpr auto getBufferSize_bytes    (void) { return _s_BufferSize_bytes;     }
   static constexpr auto getTimeStampSize_bytes (void) { return _s_TimeStampSize_bytes;  }

   void enable (VGM_T const VG);
   void disable(VGM_T const VG);

   template <Loggable... Args_T>
   inline void printf(VGM_T  const    VG,
                      str    const    Format,
                      Args_T const... Args);

private:
   void printf_Handler(VGM_T const  VG,
                       str   const  Format,
                       /*variadic*/ ...);

   void printf_Producer(str const    Format,
                        std::va_list args);

   void close_Helper(str          msg,
                     /*variadic*/ ...);

   void writeMessagesToFile(void);

   void populateFormattedTime(char * const write_Ptr) const;

   static constexpr auto _s_MaxMessageSize_bytes = 256_u32;
   static constexpr auto _s_MaxNMessagesInBuffer =  64_u32;
   static constexpr auto _s_BufferSize_bytes     = _s_MaxMessageSize_bytes * _s_MaxNMessagesInBuffer;
   static constexpr auto _s_TimeStampSize_bytes  = 34_u64;

   static_assert(std::has_single_bit(_s_MaxNMessagesInBuffer),
                 "_s_MaxNMessagesInBuffer needs to be power of 2");

   static_assert(_s_MaxNMessagesInBuffer <= (sizeof(uint64) * 8_u64),
                 "Max atomic bitfield size is 64 bits");

   static_assert(_s_MaxMessageSize_bytes > _s_TimeStampSize_bytes,
                 "No room for time stamp plus newline");

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
   std::atomic<uint64>       _readReadySlots;
   MsgSemaphore_T            _availableSem;
   MsgSemaphore_T            _messagesSem;
   std::atomic<uint32>       _writePos;
   uint32                    _readPos;
   std::atomic<WriterMode_T> _writerMode;
   TimeStampMode_T const     _TimeStampMode;
};

/** printf
 *
 * @brief Prints to the active logger.
 *
 * @tparam Args_T -- Constrained argument types.
 *
 * @param VG     -- Verbosity level.
 * @param Format -- Format string.
 * @param Args   -- Arguments.
 */
template <Loggable... Args_T>
inline void TextLogger::printf(VGM_T  const    VG,
                               str    const    Format,
                               Args_T const... Args)
{
   printf_Handler(VG, Format, Args...);
}

/** ymLog
 * 
 * @brief Prints to the active logger.
 * 
 * @tparam Args_T -- Constrained argument types.
 *
 * @param VG     -- Verbosity level.
 * @param Format -- Format string.
 * @param Args   -- Arguments.
 */
template <Loggable... Args_T>
inline void ymLog(VGM_T  const    VG,
                  str    const    Format,
                  Args_T const... Args)
{
   TextLogger::getGlobalInstance()->printf(VG, Format, Args...);
}

} // ym
