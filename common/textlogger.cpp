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

      auto const Opened = openOutfile(getFilename().get(), getOptions());
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
   if (auto expectedState = State_T::Open; _state.compare_exchange_strong(
      expectedState, State_T::Closing,
      std::memory_order_acquire,
      std::memory_order_relaxed))
   { // file opened - let's change that

      closeOutfile();
      _state.store(State_T::Closed, std::memory_order_relaxed);
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
 *       xxxxxxxxxxxx xxx:xx:xx.xxxxxx
 *       uuuuuuuuuuuu HHH:MM:SS.uuuuuu
 *
 * @param write_Ptr -- Buffer to write time stamp into.
 *
 * @returns mutstr -- Where to continue writing into the buffer (after the time stamp).
 */
auto ym::TextLogger::populateFormattedTime(
   mutstr      writePtr,
   sizet const BufSize_bytes) const -> mutstr
{
   if (getOptions() == PrintMode_T::PrependTimeStamp)
   { // print raw form of the time stamp

      YMASSERT(BufSize_bytes >= RawTimeStampTemplate.size(), Error, YM_DAH,
         "Buffer ({}) cannot fit raw time stamp ({})", BufSize_bytes, RawTimeStampTemplate.size());
   
      auto       elapsed      = _timer.getElapsedTime();
      auto const TotalTime_us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);

      auto const Result = fmt::format_to_n(
         writePtr.get(),
         RawTimeStampTemplate.size(),
         "{:012}",
         TotalTime_us.count());

      writePtr = Result.out;

      if (getOptions() == PrintMode_T::PrependHumanReadableTimeStamp)
      { // print human readable form of the time stamp

         { // temp scope
            constexpr auto TimeSize_bytes =
               RawTimeStampTemplate.size() +
               HumanReadableTimeStampTemplate.size() +
               1uz; // for space between time stamps
            YMASSERT(BufSize_bytes >= TimeSize_bytes, Error, YM_DAH,
               "Buffer ({}) cannot fit raw and human readable time stamps ({})", Size_bytes, TimeSize_bytes);
         }

         *writePtr = ' ';
         writePtr = writePtr + 1uz;

         auto const Time_hr  = std::chrono::duration_cast<std::chrono::hours>(elapsed);
         elapsed -= Time_hr;

         auto const Time_min = std::chrono::duration_cast<std::chrono::minutes>(elapsed);
         elapsed -= Time_min;

         auto const Time_sec = std::chrono::duration_cast<std::chrono::seconds>(elapsed);
         elapsed -= Time_sec;

         auto const Time_us  = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);

         auto const Result = fmt::format_to_n(
            writePtr.get(),
            HumanReadableTimeStampTemplate.size(),
            "{:03}:{:02}:{:02}.{:06}",
            Time_hr.count(),
            Time_min.count(),
            Time_sec.count(),
            Time_us.count());

         writePtr = Result.out;
      }
   }

   return writePtr;
}
