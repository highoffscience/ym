/**
 * @file    globallogger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "globallogger.h"

#include "fmt/format.h"

#include <cstdio>

/** GlobalLogger
 *
 * @brief Constructor.
 *
 * @note Explicitly clear the write flag. <https://en.cppreference.com/w/cpp/atomic/ATOMIC_FLAG_INIT>.
 * 
 * @param Filename -- Name of file to open.
 * @param Options  -- List of optional modes.
 */
ym::GlobalLogger::GlobalLogger(
   strlit    const   Filename,
   Options_T const & Options) :
      TextLogger(Filename),
      _Options {Options}
{
   _writeFlag.clear();
}

/** ~GlobalLogger
 *
 * @brief Destructor.
 */
ym::GlobalLogger::~GlobalLogger(void)
{
   close();
}

/** isOpen
 *
 * @brief Returns if outfile is open and able to be written to.
 *
 * @returns True if outfile is open and able to be written to, false otherwise.
 */
bool ym::GlobalLogger::isOpen(void) const
{
   return _state.load(std::memory_order_relaxed) == State_T::Open;
}

/** getGlobalInstancePtr
 *
 * @brief Gets the global logger instance.
 * 
 * @note Used as the global logger for the program. Not expected to close until the end.
 * @note Not thread-safe.
 *
 * @throws GlobalError -- If TextLogger instance fails to be instantiated.
 */
auto ym::GlobalLogger::getGlobalInstance(void) -> BoundPtr<GlobalLogger>
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
bool ym::GlobalLogger::open(void)
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
void ym::GlobalLogger::close(void)
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
void ym::GlobalLogger::acquireWriteAccess(void)
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
void ym::GlobalLogger::releaseWriteAccess(void)
{
   _writeFlag.clear(std::memory_order_release);
   _writeFlag.notify_one();
}

/**
 * @brief TODO
 */
void ym::GlobalLogger::producer(
   strlit const     Format,
   fmt::format_args args) noexcept
{
   // seq == slot_idx     -> ready to be written
   // seq == slot_idx + 1 -> ready to be read

   // we need to compare closing flag here and elsewhere in this function.

   // if (auto expectedState = State_T::Open; _state.compare_exchange_strong(
   //    expectedState, State_T::Closing,
   //    std::memory_order_acquire,
   //    std::memory_order_relaxed))
   // { // file opened - let's change that

   //    closeOutfile();
   //    _state.store(State_T::Closed, std::memory_order_relaxed);
   // }

   auto   const WritePos = _writePos.fetch_add(1u, std::memory_order_relaxed);
   auto * const slot_Ptr = &_slots[WritePos % _slots.size()];

   for (
      auto seqN = 0u;
      (seqN = slot_Ptr->_seqN.load(std::memory_order_acquire)) != WritePos;)
   { // wait for slot "ready to be written"
      slot_Ptr->_seqN.wait(seqN, std::memory_order_acquire);
   }

   auto * head = slot_Ptr->_msgBuffer.data();

   try
   { // attempt to write time stamp
      head = populateFormattedTime(head, slot_Ptr->_msgBuffer.size() - 1uz).get();
   }
   catch (std::exception const & E)
   { // error writing time stamp
      ymLog(VF::Errstream, "TimeStamp error in GlobalLogger producer - {}", E.what());
   }

   auto const Result = fmt::vformat_to_n( // does *not* append null terminator
      head,
      MaxMsgSize_bytes,
      Format.get(),
      args);

   if (getOptions() == PrintMode_T::KeepOriginal)
   { // line to be printed as is
      if ((Result.size + 1uz) > MaxMsgSize_bytes)
      { // truncation likely happened
         ymLog(VF::Errstream, "Truncation in GlobalLogger producer");
         *(Result.out - 1) = '\0';
      }
      else
      { // ensure null termination
         *Result.out = '\0';
      }
   }
   else
   { // mangling has occured - ensure newline
      if ((Result.size + 2uz) > MaxMsgSize_bytes)
      { // truncation likely happened
         ymLog(VF::Errstream, "Truncation in GlobalLogger producer");
         *(Result.out - 2) = '\n';
         *(Result.out - 1) = '\0';
      }
      else if (*(Result.out - 1) == '\n')
      { // newline already appended
         Result.out[0] = '\0';
      }
      else
      { // add newline
         Result.out[0] = '\n';
         Result.out[1] = '\0';
      }
   }

   slot_Ptr->_seqN.store(WritePos + 1u, std::memory_order_release); // mark as "ready to be read"
   slot_Ptr->_seqN.notify_one();
}

/**
 * @brief TODO
 */
void ym::GlobalLogger::printer(void)
{
   // seq == slot_idx     -> ready to be written
   // seq == slot_idx + 1 -> ready to be read

   auto   const ReadPos  = _readPos.load(std::memory_order_relaxed);
   auto * const slot_Ptr = &_slots[ReadPos % _slots.size()];

   for (
      auto seqN = 0u;
      (seqN = slot_Ptr->_seqN.load(std::memory_order_acquire)) != (ReadPos + 1u);)
   { // wait for slot "ready to be read"
      slot_Ptr->_seqN.wait(seqN, std::memory_order_acquire);
   }

   try
   { // attempt to write message to file
      fmt::print(_file.unwrap(), slot_Ptr->_msgBuffer.data());
   }
   catch (std::exception const & E)
   { // logic or formatting error
      ymLog(VF::Errstream, "(global logger) fmt::print encountered an error. {}", E.what());
   }

   slot_Ptr->_seqN.store(ReadPos + _slots.size(), std::memory_order_release);
   slot_Ptr->_seqN.notify_all();
   _readPos.store(ReadPos + 1u, std::memory_order_relaxed);
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

   auto const NewlineSize_bytes = (HasTimeStamp) ? 1uz : 0uz;

   auto result = fmt::vformat_to_n(
      write_Ptr,
      getMaxMsgSize_bytes() - TimeStampSize_bytes - NewlineSize_bytes,
      Format,
      args);

   if (HasTimeStamp)
   { // automatically print newline if in this mode
      *result.out = '\n';
   }

   acquireWriteAccess(); // TODO make this RAII

   if (_state.load(std::memory_order_relaxed) == State_T::Open)
   { // ok to print

      YMASSERT(result.out >= buffer, PrintError,
         [this](auto const & E) -> void {
            this->releaseWriteAccess();
            throw E;
         }, "Format to buffer did not behave as expected")

      auto const TotalWritten_bytes = static_cast<sizet>(result.out - buffer) + NewlineSize_bytes;

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
      ymLog(VF::Errstream, "Tried to print on a logger that is not opened!");
   }

   releaseWriteAccess();
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
   TextLogger * const logger_Ptr//,
   /*VG           const VG*/) :
      _logger_Ptr {logger_Ptr            },
      // _VG         {VG                    },
      _WasEnabled {false} // TODO was logger_Ptr->enable(VG)
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
