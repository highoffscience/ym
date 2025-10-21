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
 * @param Filename -- Name of file to open.
 */
ym::TextLogger::TextLogger(str const Filename) :
   _Filename {Filename}
{ }

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
   if (_vGroups.test(VG))
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
 * @throws Error -- If unenexpected pointer manipulation happens.
 * @throws Error -- If time stamp cannot fit into the buffer.
 * 
 * @param Format -- Format string.
 * @param args   -- Arguments.
 */
void ym::TextLogger::printf_Handler(
   strlit const     Format,
   fmt::format_args args)
{
   char buffer[256uz];
   auto * const write_Ptr = populateFormattedTime(buffer); // conditionally

   YMASSERT(write_Ptr >= buffer, Error, YM_DAH,
      "populateFormattedTime not returning as expected")

   auto const TimeStampSize_bytes = static_cast<sizet>(write_Ptr - buffer);

   YMASSERT(sizeof(buffer) >= TimeStampSize_bytes, Error, YM_DAH,
      "Buffer ({} bytes) not large enough to hold time stamp ({} bytes)",
      sizeof(buffer), TimeStampSize_bytes)
   
   auto const HasTimeStamp =
      getOptions() == PrintMode_T::PrependTimeStamp ||
      getOptions() == PrintMode_T::PrependHumanReadableTimeStamp;

   auto const NewlineSize_bytes = (HasTimeStamp) ? 1uz : 0uz;

   auto result = fmt::vformat_to_n(
      write_Ptr,
      sizeof(buffer) - TimeStampSize_bytes - NewlineSize_bytes,
      Format,
      args);

   if (HasTimeStamp)
   { // automatically print newline if in this mode
      *result.out = '\n';
   }

   acquireWriteAccess(); // TODO make this RAII

   if (_state.load(std::memory_order_relaxed) == State_T::Open)
   { // ok to print

      YMASSERT(result.out >= buffer, Error,
         [this](auto const & E) -> void {
            this->releaseWriteAccess();
            throw E;
         }, "Format to buffer did not behave as expected")

      auto const TotalWritten_bytes = static_cast<sizet>(result.out - buffer) + NewlineSize_bytes;

      // It is possible to ship this block to another thread/process,
      // but if no need for it just write it here. It blocks, but if
      // you're printing it's probably not a high performance task
      // anyways (see DataLogger).

      // TODO call a print_intermediate() function. It is pure virtual.
      // lite logger simply uses std::fwrite and prints to file immediately.
      // global logger writes to a waiting room buffer which a consumer reads from.
      std::ignore = std::fwrite(buffer, sizeof(char), TotalWritten_bytes, _outfile_uptr.get());
      producer(buffer, TotalWritten_bytes);

      if (getOptions() == RedirectMode_T::ToLogAndStdOut)
      { // print to console
         buffer[sizeof(buffer) - 1uz] = '\0';
         fmt::print("{}", buffer);
      }

      if (auto const WantedSize_bytes = (result.size + NewlineSize_bytes);
         WantedSize_bytes > sizeof(buffer))
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
