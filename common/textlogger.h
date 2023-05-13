/**
 * @file    textlogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "logger.h"
#include "timer.h"
#include "verbogroup.h"
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
inline void ymLog(VG     const    VG,
                  str    const    Format,
                  Args_T const... Args);

// TODO implement

inline void ymLogEnable (VG const VG);
inline void ymLogDisable(VG const VG);

inline class ScopedEnable ymLogPushEnable(VG const VG);

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
   /** PrintMode_T
    *
    * @brief Mode to determine how to mangle the printable message.
    */
   enum class PrintMode_T : uint32
   {
      KeepOriginal,
      PrependTimeStamp
   };

   /** RedirectMode_T
    * 
    * @brief Specifies what streams to pipe the output to.
    */
   enum class RedirectMode_T : uint32
   {
      ToStdOut,
      ToStdErr,
      ToLog
   };

   explicit TextLogger(void);
   ~TextLogger(void);

   YM_NO_COPY  (TextLogger)
   YM_NO_ASSIGN(TextLogger)

   YM_DECL_YMCEPT(TextLoggerError)
   YM_DECL_YMCEPT(TextLoggerError, TextLoggerError_GlobalFailureToOpen)
   YM_DECL_YMCEPT(TextLoggerError, TextLoggerError_FailureToOpen)

   static TextLogger * getGlobalInstancePtr      (void);
   static TextLogger * getGlobalStdErrInstancePtr(void);

   bool isOpen(void) const;

   bool open(PrintMode_T    const PrintMode,
             RedirectMode_T const RedirectMode);
   bool open(FilenameMode_T const FilenameMode,
             PrintMode_T    const PrintMode,
             RedirectMode_T const RedirectMode,
             str            const Filename);
   void close(void);

   static constexpr auto getMaxMessageSize_bytes(void) { return _s_MaxMessageSize_bytes; }
   static constexpr auto getMaxNMessagesInBuffer(void) { return _s_MaxNMessagesInBuffer; }
   static constexpr auto getBufferSize_bytes    (void) { return _s_BufferSize_bytes;     }
   static constexpr auto getTimeStampSize_bytes (void) { return _s_TimeStampSize_bytes;  }

   /** ScopedEnable
    * 
    * @brief Allows managed temporary enabling of a verbosity group.
    * 
    * @note Uses RAII to storing/restoring enabling verbosity groups.
    * 
    * @note The return value from @ref TextLogger::pushEnable will need to be explicitly
    *       stored, ie.
    *       auto const SC = ymLogPushEnable(VG);
    *       even if SC is not used, since the destructor has side effects. Simply calling
    *       pushEnable will result in the ScopedEnable structure being deleted immediately.
    */
   class ScopedEnable
   {
   public:
      explicit ScopedEnable(TextLogger * const logger_Ptr,
                            VG           const VG);
      ~ScopedEnable(void);

      void popEnable(void) const;

   private:
      TextLogger * const _logger_Ptr;
      VG           const _VG;
      bool         const _WasEnabled;
   };

   bool enable (VG const VG);
   bool disable(VG const VG);

   ScopedEnable pushEnable(VG const VG);

   template <Loggable... Args_T>
   inline void printf(VG     const    VG,
                      str    const    Format,
                      Args_T const... Args);

private:
   void printf_Handler(VG    const  VG,
                       str   const  Format,
                       /*variadic*/ ...);

   void printf_Producer(str const    Format,
                        std::va_list args);

   bool open_Helper(bool const Opened);

   void close_Helper(str          msg,
                     /*variadic*/ ...);

   void writeMessagesToFile(void);

   void populateFormattedTime(char * const write_Ptr) const;

   static constexpr auto _s_MaxMessageSize_bytes = 256_u32;
   static constexpr auto _s_MaxNMessagesInBuffer =  64_u32;
   static constexpr auto _s_BufferSize_bytes     = _s_MaxMessageSize_bytes * _s_MaxNMessagesInBuffer;
   static constexpr auto _s_TimeStampSize_bytes  = 33_u32;

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
   using VGroups_T      = std::array<std::atomic<uint8>, VerboGroup::getNGroups()>;

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
   PrintMode_T               _printMode;
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
inline void TextLogger::printf(VG     const    VG,
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
inline void ymLog(VG     const    VG,
                  str    const    Format,
                  Args_T const... Args)
{
   TextLogger::getGlobalInstancePtr()->printf(VG, Format, Args...);
}

} // ym
