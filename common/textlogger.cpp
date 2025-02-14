/**
 * @file    textlogger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "textlogger.h"

#include "fmt/core.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <memory>
#include <utility>

/** TextLogger
 *
 * @brief Constructor.
 */
ym::TextLogger::TextLogger(void)
   : _buffer         {'\0'                        },
     _vGroups        {/*default*/                 },
     _writer         {/*default*/                 },
     _timer          {/*default*/                 },
     _readReadySlots {0_u64                       },
     _availableSem   {static_cast<std::ptrdiff_t>(
                      getMaxNMessagesInBuffer()  )},
     _messagesSem    {0_i32                       },
     _writePos       {0_u32                       },
     _readPos        {0_u32                       },
     _writerMode     {WriterMode_T::Closed        },
     _printMode      {PrintMode_T::KeepOriginal   }
{
   static_assert(sizeof(std::ptrdiff_t) > sizeof(getMaxNMessagesInBuffer()),
      "Potential overflow of signed value");
}

/** ~TextLogger
 *
 * @brief Destructor.
 */
ym::TextLogger::~TextLogger(void)
{
   close();
}

/** isOpen
 *
 * @brief Returns if outfile is open and able to be written to.
 *
 * @returns True if outfile is open and able to be written to, false otherwise.
 */
bool ym::TextLogger::isOpen(void) const
{
   return _writerMode.load(std::memory_order_relaxed) == WriterMode_T::Open;
}

/** getGlobalInstancePtr
 *
 * @brief Gets the global text logger instance.
 * 
 * @throws TextLoggerError_GlobalFailureToOpen -- If TextLogger instance fails to be instantiated.
 * @throws TextLoggerError_GlobalFailureToOpen -- If attempting to open throws.
 * @throws TextLoggerError_GlobalFailureToOpen -- If attempting to open is unsuccessful.
 *
 * @note Used as the global logger for the program. Not expected to close until the end.
 */
auto ym::TextLogger::getGlobalInstancePtr(void) -> TextLogger *
{
   static TextLogger * instance_ptr = nullptr;

   if (!instance_ptr)
   { // file not already opened - open it
      instance_ptr = new TextLogger();

      TextLoggerError_GlobalFailureToOpen::check(instance_ptr,
         "Global instance failed to be created");

      auto opened = false; // until told otherwise

      try
      { // attempt to open the file
         opened = instance_ptr->open(
            #if defined(YM_PRINT_TO_SCREEN)
               PrintMode_T::PrependTimeStamp,
               RedirectMode_T::ToStdOut
            #else
               FilenameMode_T::AppendTimeStamp,
               PrintMode_T::PrependTimeStamp,
               RedirectMode_T::ToLog,
               "global.txt"
            #endif // YM_PRINT_TO_SCREEN
         );
      }
      catch (std::exception const & E)
      { // clean up memory before rethrowing
         delete instance_ptr;
         instance_ptr = nullptr;

         TextLoggerError_GlobalFailureToOpen::check(false,
            "Error in opening global file (%s)", E.what());
      }

      if (!opened)
      { // something went wrong - clean up memory
         delete instance_ptr;
         instance_ptr = nullptr;

         TextLoggerError_GlobalFailureToOpen::check(false,
            "Global instance failed to open");
      }
   }

   return instance_ptr; // guaranteed not null
}

/** open
 *
 * @brief Opens and prepares the logger to be written to.
 * 
 * @throws TextLoggerError_FailureToOpen -- If redirect mode doesn't point to a static stream.
 * @throws LoggerError_FailureToOpen     -- If the file already exists.
 * 
 * @throws TextLoggerError_FailureToOpen -- If filename is specified but ToStdOut mode is requested.
 * @throws TextLoggerError_FailureToOpen -- If filename is specified but ToStdErr mode is requested.
 * @throws TextLoggerError_FailureToOpen -- If filename is not specified but ToLog mode is requested.
 * @throws TextLoggerError_FailureToOpen -- If RedirectMode is invalid.
 * 
 * @param PrintMode    -- Mode to determine how to mangle the printable message.
 * @param RedirectMode -- Specifies what streams to pipe the output to.
 * 
 * @returns bool -- Whether the outfile was opened successfully, false otherwise.
 */
bool ym::TextLogger::open(PrintMode_T    const PrintMode,
                          RedirectMode_T const RedirectMode)
{
   TextLoggerError_FailureToOpen::check(
      RedirectMode == RedirectMode_T::ToStdOut ||
      RedirectMode == RedirectMode_T::ToStdErr,
      "Redirect mode %u must specify a filename", std::to_underlying(RedirectMode)
   );

   return open(FilenameMode_T::KeepOriginal, PrintMode, RedirectMode, "");
}

/** open
 *
 * @brief Opens and prepares the logger to be written to.
 *
 * @throws LoggerError_FailureToOpen -- If openOutfile() fails.
 * 
 * @throws TextLoggerError_FailureToOpen -- If filename is specified but ToStdOut mode is requested.
 * @throws TextLoggerError_FailureToOpen -- If filename is specified but ToStdErr mode is requested.
 * @throws TextLoggerError_FailureToOpen -- If filename is not specified but ToLog mode is requested.
 * @throws TextLoggerError_FailureToOpen -- If RedirectMode is invalid.
 * 
 * @param FilenameMode -- Mode to determine how to mangle the filename.
 * @param PrintMode    -- Mode to determine how to mangle the printable message.
 * @param RedirectMode -- Specifies what streams to pipe the output to.
 * @param Filename     -- Name of file to open.
 * 
 * @returns bool -- Whether the outfile was opened successfully, false otherwise.
 */
bool ym::TextLogger::open(FilenameMode_T const FilenameMode,
                          PrintMode_T    const PrintMode,
                          RedirectMode_T const RedirectMode,
                          str            const Filename)
{
   _printMode = PrintMode;

   auto opened = false; // until told otherwise

   switch (RedirectMode)
   {
      case RedirectMode_T::ToStdOut:
      {
         TextLoggerError_FailureToOpen::check(!*Filename,
            "File '%s' attempted to open with StdOut only option", Filename.get());
         opened = open_Helper(openOutfile(stdout));
         break;
      }

      case RedirectMode_T::ToStdErr:
      {
         TextLoggerError_FailureToOpen::check(!*Filename,
            "File '%s' attempted to open with StdErr only option", Filename.get());
         opened = open_Helper(openOutfile(stderr));
         break;
      }

      case RedirectMode_T::ToLog:
      {
         TextLoggerError_FailureToOpen::check(*Filename,
            "Attempted to open empty file");
         opened = open_Helper(openOutfile(Filename, FilenameMode));
         break;
      }

      default:
      {
         TextLoggerError_FailureToOpen::check(false,
            "Unaccounted for redirect mode %u", std::to_underlying(RedirectMode));
         break;
      }
   }

   return opened;
}

/** close
 *
 * @brief Closes the outfile and shuts the logger down.
 * 
 * @throws TextLoggerError_ProducerConsumerError -- If close_Helper() fails.
 */
void ym::TextLogger::close(void)
{
   auto       expected = WriterMode_T::Open;
   auto const Desired  = WriterMode_T::PreparingToClose;

   if (_writerMode.compare_exchange_strong(expected, Desired,
                                           std::memory_order_acquire,  // success
                                           std::memory_order_relaxed)) // failure
   { // file open

      close_Helper("File closing...");
      _writerMode.store(WriterMode_T::Closing, std::memory_order_release);
      _writer.join(); // wait until all messages have been written before closing the file
      closeOutfile();
      _writerMode.store(WriterMode_T::Closed, std::memory_order_relaxed);
   }
}

/** open_Helper
 * 
 * @brief Starts the consumer thread of the outfile was successfully opened.
 * 
 * @param Opened -- Whether or not the outfile was successfully opened.
 * 
 * @returns bool -- Whether or not the outfile was successfully opened.
 */
bool ym::TextLogger::open_Helper(bool const Opened)
{
   if (Opened)
   { // file opened successfully
      _writer     = std::thread(&TextLogger::writeMessagesToFile, this); // starts the thread
      _writerMode = WriterMode_T::Open;
   }

   return Opened;
}

/** close_Helper
 * 
 * @brief Wrapper (helper) function to call printf_Producer().
 * 
 * @throws TextLoggerError_ProducerConsumerError -- If printf_Producer() fails.
 * 
 * @param msg -- Message to print.
 * @param ... -- Arguments.
 */
void ym::TextLogger::close_Helper(str          msg,
                                  /*variadic*/ ...)
{
   std::va_list args;
   va_start(args, msg);
   printf_Producer(msg, args); // this call is necessary to wake the semaphore and check the closing flag
   va_end(args);
}

/** enable
 *
 * @brief Enables specified verbosity group.
 *
 * @param VG -- Verbosity group to enable.
 * 
 * @returns bool -- If previously enabled before this call.
 */
bool ym::TextLogger::enable(VG const VG)
{
   using VGM = VerboGroupMask;
   return _vGroups[VGM::getGroup(VG)].fetch_or(VGM::getMaskAsByte(VG), std::memory_order_relaxed);
}

/** disable
 *
 * @brief Disables specified verbosity group.
 *
 * @param VG -- Verbosity group to disable.
 * 
 * @returns bool -- If previously enabled before this call.
 */
bool ym::TextLogger::disable(VG const VG)
{
   using VGM = VerboGroupMask;
   return _vGroups[VGM::getGroup(VG)].fetch_and(~VGM::getMaskAsByte(VG), std::memory_order_relaxed);
}

/** pushEnable
 * 
 * @brief Enables given verbosity group only in the current scope.
 * 
 * @param VG -- Verbosity group.
 * 
 * @returns ScopedEnable -- RAII mechanism that only keeps the enable VG while in scope.
 */
auto ym::TextLogger::pushEnable(VG const VG) -> ScopedEnable
{
   return ScopedEnable(this, VG);
}

/** printf_Handler
 *
 * @brief Conditionally prints the requested message.
 *
 * @note printf_Producer is not marked noexcept because terminate should not be called.
 *
 * @throws TextLoggerError_ProducerConsumerError -- If printf_Producer() if fails.
 * 
 * @param VG     -- Verbosity group.
 * @param Format -- Format string.
 * @param ...    -- Arguments.
 */
void ym::TextLogger::printf_Handler(VG    const  VG,
                                    str   const  Format,
                                    /*variadic*/ ...)
{
   if (isOpen())
   { // ok to print

      using VGM = VerboGroupMask;
      auto const IsEnabled = (_vGroups[VGM::getGroup(VG)] & VGM::getMaskAsByte(VG)) > 0_u8;

      if (IsEnabled)
      { // verbose enough to print this message

         std::va_list args;
         va_start(args, Format);
         // TODO should be marked as noexcept
         printf_Producer(Format, args); // doesn't throw so va_end will be called
         va_end(args);
      }
   }
}

/** printf_Handler
 *
 * @brief Prints the requested message to the internal buffer.
 *
 * @note The system call to get the timestamp is usually optimized at runtime.
 * 
 * @param Format -- Format string.
 * @param args   -- Arguments.
 */
void ym::TextLogger::printf_Handler(
   rawstr const Msg,
   uint64 const Size_bytes)
{
   while (_writeFlag.test_and_set(std::memory_order_acquire))
   {
   #if (YM_CPP_STANDARD >= 20)
      _writeFlag.wait(true, std::memory_order_relaxed);
   #else
      std::this_thread::yield();
   #endif
   }

   if (_printMode == PrintMode_T::PrependTimeStamp)
   { // print message with time stamp

      char timeStampBuffer[getTimeStampSize_bytes()];
      populateFormattedTime(timeStampBuffer, getTimeStampSize_bytes());

      nCharsWrittenInTheory = std::fprintf(_outfile_uptr.get(), "%s: %s\n", timeStampBuffer, Read_Ptr);
   }
   else
   { // print message plain
      nCharsWrittenInTheory = std::fprintf(_outfile_uptr.get(), "%s", Read_Ptr);
   }

   std::fwrite(Msg, sizeof(char), MsgSize_bytes, _outfile_Ptr);

   _writeFlag.clear(std::memory_order_released);

#if (YM_CPP_STANDARD >= 20)
   _writeFlag.notify_one();
#endif
}

/** writeMessagesToFile
 *
 * @brief Writes all pending messages in the buffer to the outfile.
 *
 * @note This is the consumer thread.
 * 
 * @note There's also full-blown defense sections where your ally has to stop and inject a
 *       a virus into three separate pillars while endless waves of enemies try to kill them
 *       because obviously they don't want those pillars to have viruses injected into them.
 * 
 * @throws TextLoggerError_ProducerConsumerError -- If _messagesSem fails to acquire.
 * @throws TextLoggerError_ProducerConsumerError -- If _availableSem fails to release.
 */
void ym::TextLogger::writeMessagesToFile(void)
{
   auto writerEnabled = true;

   while (writerEnabled)
   { // wait for messages to print while logger is still active

      auto nMsgs = 0_u32;
      do
      { // wait until the next message is ready
         try
         { // try to acquire
            _messagesSem.acquire(); // _readReadySlots has been updated
         }
         catch (std::exception const & E)
         { // post what went wrong and throw
            TextLoggerError_ProducerConsumerError::check(false,
               "Msgs sem failed to acquire (%s)", E.what());
         }

         nMsgs++;
      }
      while ( !( (1_u64 << _readPos) & _readReadySlots.load(std::memory_order_acquire) ) );

      while (nMsgs > 0_u32)
      { // write pending messages

         auto         const ReadPos  = _readPos % getMaxNMessagesInBuffer();
         auto const * const Read_Ptr = _buffer + (ReadPos * getMaxMessageSize_bytes());
         _readPos++; // doesn't wrap - needs to keep pace with _writePos which doesn't wrap

         auto nCharsWrittenInTheory = -1_i32; // default to error value

         if (_printMode == PrintMode_T::PrependTimeStamp)
         { // print message with time stamp
            char timeStampBuffer[getTimeStampSize_bytes()] = {'\0'};
            populateFormattedTime(timeStampBuffer);

            nCharsWrittenInTheory = std::fprintf(_outfile_uptr.get(), "%s: %s\n", timeStampBuffer, Read_Ptr);
         }
         else
         { // print message plain
            nCharsWrittenInTheory = std::fprintf(_outfile_uptr.get(), "%s", Read_Ptr);
         }

         if (nCharsWrittenInTheory < 0_i32)
         { // fwrite hit an internal error
            printfInternalError("std::fprintf failed with error code %d! "
                                "Message was '%s'\n",
                                nCharsWrittenInTheory,
                                Read_Ptr);

            // don't fail here - just keep going
         }

         nMsgs--;

         try
         { // try to release
            _availableSem.release();
         }
         catch (std::exception const & E)
         { // post what went wrong and throw
            TextLoggerError_ProducerConsumerError::check(false,
               "Avail sem failed to release (%s)", E.what());
         }
      }

      if (_writerMode.load(std::memory_order_relaxed) == WriterMode_T::Closing)
      { // close requested
         if (_readPos == _writePos.load(std::memory_order_relaxed))
         { // no more messages in buffer
            writerEnabled = false;
         }
      }
   }
}

/** populateFormattedTime
 *
 * @brief Writes the elapsed time in the specified buffer.
 * 
 * @note Does *not* write null terminator.
 * 
 * @note Not to be confused with Logger::populateFilenameTimeStamp.
 *
 * @note Returns the current time, in microseconds, since the creation of the log in the format
 *       (xxxxxxxxxxxx) xxx:xx:xx.xxxxxx
 *
 * @param write_Ptr  -- Buffer to write time stamp into.
 * @param Size_bytes -- Size of buffer.
 */
void ym::TextLogger::populateFormattedTime(
   char * const write_Ptr,
   uint64 const Size_bytes) const
{
   auto elapsed = _timer.getElapsedTime();

   auto const TotalTime_us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
   auto const Time_hr      = std::chrono::duration_cast<std::chrono::hours>       (elapsed);
   elapsed -= Time_hr;
   auto const Time_min     = std::chrono::duration_cast<std::chrono::minutes>     (elapsed);
   elapsed -= Time_min;
   auto const Time_sec     = std::chrono::duration_cast<std::chrono::seconds>     (elapsed);
   elapsed -= Time_sec;
   auto const Time_us      = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);

   // Use fmt to format directly into the pre-allocated buffer
   [[maybe_unused]] auto const Result = fmt::fprintf(
      write_Ptr,
      "({:012}) {:03}:{:02}:{:02}.{:06}",
      TotalTime_us.count(),
      Time_hr.count(),
      Time_min.count(),
      Time_sec.count(),
      Time_us.count());
}

/** ScopedEnable
 * 
 * @brief Constructor.
 * 
 * @note Enables upon construction.
 * 
 * @param logger_Ptr -- Logger instance to enable VG for.
 * @param VG         -- Verbosity group.
 */
ym::TextLogger::ScopedEnable::ScopedEnable(
   TextLogger * const logger_Ptr,
   VG           const VG)
   : _logger_Ptr {logger_Ptr            },
     _VG         {VG                    },
     _WasEnabled {logger_Ptr->enable(VG)}
{
}

/** ~ScopedEnable
 * 
 * @brief Destructor.
 * 
 * @note Disables upon exit.
 */
ym::TextLogger::ScopedEnable::~ScopedEnable(void)
{
   popEnable();
}

/** popEnable
 * 
 * @brief Restores the enable state of the stored VG.
 */
void ym::TextLogger::ScopedEnable::popEnable(void) const
{
   if (!_WasEnabled)
   { // disable
      _logger_Ptr->disable(_VG);
   }
}
