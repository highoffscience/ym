/**
 * @file    textlogger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "textlogger.h"

#include "fmt/format.h"

#include <chrono>
#include <cstdio>

/** TextLogger
 *
 * @brief Constructor.
 *
 * @note Explicitly clear the write flag. <https://en.cppreference.com/w/cpp/atomic/ATOMIC_FLAG_INIT>.
 * 
 * @param Filename -- Name of file to open.
 * @param Options  -- List of optional modes.
 */
ym::TextLogger::TextLogger(
   str       const   Filename,
   Options_T const & Options) :
      _Filename {Filename},
      _Options  {Options }
{
   _writeFlag.clear();
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
   return _state.load(std::memory_order_relaxed) == State_T::Open;
}

/** getGlobalInstancePtr
 *
 * @brief Gets the global text logger instance.
 * 
 * @note Used as the global logger for the program. Not expected to close until the end.
 * @note Not thread-safe.
 *
 * @throws GlobalError -- If TextLogger instance fails to be instantiated.
 */
auto ym::TextLogger::getGlobalInstancePtr(void) -> bptr<TextLogger>
{
   if (!_s_globalInstance_ptr)
   { // file not already opened - open it

      _s_globalInstance_ptr = new TextLogger("logs/global.txt");
      YMASSERT(_s_globalInstance_ptr, GlobalError, YM_DAH, "Global instance failed to be created");

      auto const Opened = _s_globalInstance_ptr->open();
      YMASSERT(Opened, GlobalError,
         [](auto const & E) -> void {
            delete _s_globalInstance_ptr;
            _s_globalInstance_ptr = nullptr;
            throw E;
         }, "Global instance failed to open");
   }

   return tbptr(_s_globalInstance_ptr); // guaranteed not null
}

/** open
 *
 * @brief Opens and prepares the logger to be written to.
 * 
 * @returns bool -- Whether the outfile was opened successfully, false otherwise.
 */
bool ym::TextLogger::open(void)
{
   auto expectedState = State_T::Closed;

   if (_state.compare_exchange_strong(
      expectedState, State_T::Opening,
      std::memory_order_acquire,
      std::memory_order_relaxed))
   { // file not opened - let's do that

      auto const Opened = openOutfile(getFilename().get(), _Options);
      expectedState = Opened ? State_T::Open : State_T::Closed;
      _state.store(expectedState, std::memory_order_relaxed);
   }

   return expectedState == State_T::Open;
}

/** close
 *
 * @brief Closes the outfile and shuts the logger down.
 */
void ym::TextLogger::close(void)
{
   acquireWriteAccess();

   if (auto expectedState = State_T::Open; _state.compare_exchange_strong(
      expectedState, State_T::Closing,
      std::memory_order_acquire,
      std::memory_order_relaxed))
   { // file opened - let's change that

      closeOutfile();
      _state.store(State_T::Closed, std::memory_order_relaxed);
   }

   releaseWriteAccess();
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

/** acquireWriteAccess
 * 
 * @brief Acquires the write flag.
 */
void ym::TextLogger::acquireWriteAccess(void)
{
   while (_writeFlag.test_and_set(std::memory_order_acquire))
   { // wait until the other thread is done
      _writeFlag.wait(true, std::memory_order_relaxed);
   }
}

/** releaseWriteAccess
 * 
 * @brief Releases the write flag.
 */
void ym::TextLogger::releaseWriteAccess(void)
{
   _writeFlag.clear(std::memory_order_release);
   _writeFlag.notify_one();
}

/** printf_Handler
 *
 * @brief Conditionally prints the requested message.
 *
 * @throws Whatever print_Handler(Format, args) throws.
 * 
 * @param VG     -- Verbosity group.
 * @param Format -- Format string.
 * @param ...    -- Arguments.
 */
void ym::TextLogger::printf_Handler(
   VG     const     VG,
   strlit const     Format,
   fmt::format_args args)
{
   using VGM = VerboGroupMask;
   auto const IsEnabled = (_vGroups[VGM::getGroup(VG)] & VGM::getMaskAsByte(VG)) > 0_u8;

   if (IsEnabled)
   { // verbose enough to print this message
      printf_Handler(Format, args); // TODO should take VG here too and add debug, warning, or error labels,
                                    // if applicable (rename func of course, it is already overloaded).
   }
}

/** printf_Handler
 *
 * @brief Prints the requested message to the internal buffer.
 *
 * @note The system call to get the timestamp is usually optimized at runtime.
 *
 * @throws PrintError -- If unenexpected pointer manipulation happens.
 * @throws PrintError -- If time stamp cannot fit into the buffer.
 * 
 * @param Format -- Format string.
 * @param args   -- Arguments.
 */
void ym::TextLogger::printf_Handler(
   strlit const     Format,
   fmt::format_args args)
{
   char buffer[getMaxMsgSize_bytes()]{}; // unnecessary init?
   auto * const write_Ptr = populateFormattedTime(buffer); // conditionally

   YMASSERT(write_Ptr >= buffer, PrintError, YM_DAH,
      "populateFormattedTime not returning as expected")

   auto const TimeStampSize_bytes = static_cast<std::size_t>(write_Ptr - buffer);

   YMASSERT(getMaxMsgSize_bytes() >= TimeStampSize_bytes, PrintError, YM_DAH,
      "Buffer ({} bytes) not large enough to hold time stamp ({} bytes)",
      getMaxMsgSize_bytes(), TimeStampSize_bytes)
   
   auto const HasTimeStamp =
      getOptions() == PrintMode_T::PrependTimeStamp ||
      getOptions() == PrintMode_T::PrependHumanReadableTimeStamp;

   auto const NewlineSize_bytes = std::size_t((HasTimeStamp) ? 1u : 0u);

   auto result = fmt::vformat_to_n(
      write_Ptr,
      getMaxMsgSize_bytes() - TimeStampSize_bytes - NewlineSize_bytes,
      Format,
      args);

   if (HasTimeStamp)
   { // automatically print newline if in this mode
      *result.out = '\n';
   }

   acquireWriteAccess(); // make this RAII

   if (_state.load(std::memory_order_relaxed) == State_T::Open)
   { // ok to print

      YMASSERT(result.out >= buffer, PrintError,
         [this](auto const & E) -> void {
            this->releaseWriteAccess();
            throw E;
         }, "Format to buffer did not behave as expected")

      auto const TotalWritten_bytes = static_cast<std::size_t>(result.out - buffer) + NewlineSize_bytes;

      // It is possible to ship this block to another thread/process,
      // but if no need for it just write it here. It blocks, but if
      // you're printing it's probably not a high performance task
      // anyways (see DataLogger).

      std::fwrite(buffer, sizeof(char), TotalWritten_bytes, _outfile_uptr.get());

      if (getOptions() == RedirectMode_T::ToLogAndStdOut)
      { // print to console
         buffer[getMaxMsgSize_bytes() - std::size_t(1u)] = '\0';
         fmt::print("{}", buffer);
      }

      if (auto const WantedSize_bytes = (result.size + NewlineSize_bytes);
         WantedSize_bytes > getMaxMsgSize_bytes())
      { // overflow

         releaseWriteAccess();

         if (HasTimeStamp)
         { // alert user which line overflowed
            buffer[RawTimeStampTemplate.size()] = '\0';
            printf_Handler("OVERFLOW message @ time stamp {}", fmt::make_format_args(buffer));
         }
         else
         { // alert user a message overflowed
            printf_Handler("OVERFLOW on a previous message", {});
         }
      }
   }
   else
   { // *not* ok to print
      fmt::print("WARNING: Tried to print on a logger that is not opened!");
   }

   releaseWriteAccess();
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
 *       xxxxxxxxxxxx xxx:xx:xx.xxxxxx
 *       uuuuuuuuuuuu HHH:MM:SS.uuuuuu
 *
 * @param write_Ptr -- Buffer to write time stamp into.
 *
 * @returns char * -- Where to continue writing into the buffer (after the time stamp).
 */
char * ym::TextLogger::populateFormattedTime(char * write_ptr) const
{
   if (getOptions() == PrintMode_T::PrependHumanReadableTimeStamp)
   { // print raw form of the time stamp
   
      auto       elapsed      = _timer.getElapsedTime();
      auto const TotalTime_us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);

      auto const Result = fmt::format_to_n(
         write_ptr,
         RawTimeStampTemplate.size(),
         "{:012}",
         TotalTime_us.count());

      write_ptr = Result.out;

      if (getOptions() == PrintMode_T::PrependHumanReadableTimeStamp)
      { // print human readable form of the time stamp

         auto const Time_hr   = std::chrono::duration_cast<std::chrono::hours>       (elapsed);
         elapsed -= Time_hr;

         auto const Time_min  = std::chrono::duration_cast<std::chrono::minutes>     (elapsed);
         elapsed -= Time_min;

         auto const Time_sec  = std::chrono::duration_cast<std::chrono::seconds>     (elapsed);
         elapsed -= Time_sec;

         auto const Time_us   = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);

         auto const Result = fmt::format_to_n(
            write_ptr,
            HumanReadableTimeStampTemplate.size(),
            " {:03}:{:02}:{:02}.{:06}: ",
            Time_hr.count(),
            Time_min.count(),
            Time_sec.count(),
            Time_us.count());

         write_ptr = Result.out;
      }
   }

   return write_ptr;
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
   VG           const VG) :
      _logger_Ptr {logger_Ptr            },
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
