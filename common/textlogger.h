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
#include "ymerror.h"

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

template <Loggable... Args_T>
inline void ymLogToStdErr(str    const    Format,
                          Args_T const... Args);

inline bool ymLogEnable (VG const VG);
inline bool ymLogDisable(VG const VG);

// implemented below - ScopedEnable isn't yet defined
// inline class ScopedEnable ymLogPushEnable(VG const VG);

/* -------------------------------------------------------------------------- */

/** TextLogger
 *
 * @brief Logs text to the given outfile - similary to std::fprintf.
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

   YM_DECL_YMERROR(TextLoggerError)
   YM_DECL_YMERROR(TextLoggerError, TextLoggerError_GlobalFailureToOpen)
   YM_DECL_YMERROR(TextLoggerError, TextLoggerError_FailureToOpen)
   YM_DECL_YMERROR(TextLoggerError, TextLoggerError_ProducerConsumerError)

   static TextLogger * getGlobalInstancePtr(void);

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
    *       auto const SE = ymLogPushEnable(VG);
    *       even if SE is not used, since the destructor has side effects. Simply calling
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

/** ymLogToStdErr
 *
 * @brief Print to stderr. Usually as a last ditch effort to record something.
 * 
 * @tparam Args_T -- Argument types.
 *
 * @param Format -- Format string.
 * @param Args   -- Arguments.
 */
template <Loggable... Args_T>
inline void ymLogToStdErr(str    const    Format,
                          Args_T const... Args)
{
   std::fprintf(stderr, Format, Args...);
}

/** ymLogEnable
 * 
 * @brief Enables specified verbosity group for the global logger.
 *
 * @param VG -- Verbosity group to enable.
 * 
 * @returns bool -- If previously enabled before this call.
 */
inline bool ymLogEnable(VG const VG)
{
   return TextLogger::getGlobalInstancePtr()->enable(VG);
}

/** ymLogDisable
 * 
 * @brief Disables specified verbosity group for the global logger.
 *
 * @param VG -- Verbosity group to disable.
 * 
 * @returns bool -- If previously enabled before this call.
 */
inline bool ymLogDisable(VG const VG)
{
   return TextLogger::getGlobalInstancePtr()->disable(VG);
}

/** ymLogPushEnable
 * 
 * @brief Enables given verbosity group only in the current scope for the global logger.
 * 
 * @throws TODO
 * 
 * @param VG -- Verbosity group.
 * 
 * @returns ScopedEnable -- RAII mechanism that only keeps the enable VG while in scope.
 */
inline TextLogger::ScopedEnable ymLogPushEnable(VG const VG)
{
   return TextLogger::getGlobalInstancePtr()->pushEnable(VG);
}

} // ym
