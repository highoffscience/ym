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
 * @param Filename -- Name of file to open.
 * @param Options  -- List of optional modes.
 */
ym::GlobalLogger::GlobalLogger(
   strlit    const   Filename,
   Options_T const & Options) noexcept :
      TextLogger(Filename),
      _Options {Options}
{
   for (auto i = 0u; i < _slots.size(); i++)
   { // init slot sequence numbers
      _slots[i]._seqN.store(i, std::memory_order_relaxed);
   }

   // TODO enable groups like Global, Warning, Error, etc.

   open();
}

/** ~GlobalLogger
 *
 * @brief Destructor.
 */
ym::GlobalLogger::~GlobalLogger(void) noexcept
{
   close();
}

/** isOpen
 *
 * @brief Returns if outfile is open and able to be written to.
 *
 * @returns True if outfile is open and able to be written to, false otherwise.
 */
bool ym::GlobalLogger::isOpen(void) const noexcept
{
   return _state.load(std::memory_order_relaxed) == State_T::Open;
}

/** getGlobalInstance
 *
 * @brief Gets the global logger instance.
 *
 * @note Used as the global logger for the program. Not expected to close until the end.
 *
 * @note Uses Meyers Singleton implementation.
 */
auto ym::GlobalLogger::getGlobalInstance(void) noexcept -> BoundPtr<GlobalLogger>
{
   static GlobalLogger s_instance("logs/global.txt");
   return BoundPtr(&s_instance, ym_AssumePtrNotNull{});
}

/** open
 *
 * @brief Opens and prepares the logger to be written to.
 *
 * @returns bool -- Whether the outfile was opened successfully, false otherwise.
 */
bool ym::GlobalLogger::open(void) noexcept
{
   auto expectedState = State_T::Closed;

   if (_state.compare_exchange_strong(
      expectedState, State_T::Opening,
      std::memory_order_acquire,
      std::memory_order_relaxed))
   { // file not opened - let's do that
      auto const Opened = openOutfile(getFilename().get());
      expectedState = Opened ? State_T::Open : State_T::Closed;
      _consumer = std::thread(&GlobalLogger::printer, this);
      _state.store(expectedState, std::memory_order_release);
      _state.notify_all(); // someone maybe waiting... defensive if anything
   }

   return isOpen();
}

/** close
 *
 * @brief Closes the outfile and shuts the logger down.
 *
 * @note Only one caller of close() should initiate shutdown, everyone else will wait
 *       until the logger closes before returning, so when execution returns to the caller
 *       tthe logger is indeed closed.
 */
void ym::GlobalLogger::close(void) noexcept
{
   if (auto expectedState = State_T::Open; _state.compare_exchange_strong(
      expectedState, State_T::Closing,
      std::memory_order_release,
      std::memory_order_acquire))
   { // file opened - let's change that
      auto   const ReadPos  = _readPos.load(std::memory_order_relaxed);
      auto * const slot_Ptr = &_slots[ReadPos % _slots.size()];
      // we use -1 to simulate an out-of-order sequence counting to signal to the consumer thread
      // to check the closing state and not attempt to write anything from this current slot
      slot_Ptr->_seqN.store(ReadPos - 1u, std::memory_order_release);
      slot_Ptr->_seqN.notify_one(); // wake consumer
      _consumer.join();
      closeOutfile();
      _state.store(State_T::Closed, std::memory_order_release);
      _state.notify_all(); // someone else might be waiting
   }
   else
   { // already closing or closed
      while (_state.load(std::memory_order_acquire) != State_T::Closed)
      { // hang out here until we are closed
         _state.wait(State_T::Closed, std::memory_order_acquire);
      }
   }
}

/** producer
 *
 * @brief Writes the Format string to file.
 *
 * @param Format -- Format string.
 * @param args   -- Arguments.
 */
void ym::GlobalLogger::producer(
   strlit const     Format,
   fmt::format_args args) noexcept
{
   // seq == slot_idx     -> ready to be written
   // seq == slot_idx + 1 -> ready to be read

   if (_state.load(std::memory_order_acquire) != State_T::Open)
   { // not open for business
      ymLog(VF::Errstream, "Attempted to write to closed GlobalLogger! - {}", Format);
      return; // exit early
   }

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

/** printer
 *
 * @brief Writes messages from the queue.
 */
void ym::GlobalLogger::printer(void) noexcept
{
   // seq == slot_idx     -> ready to be written
   // seq == slot_idx + 1 -> ready to be read

   while (true)
   { // consume messages
      auto   const ReadPos  = _readPos.load(std::memory_order_relaxed);
      auto * const slot_Ptr = &_slots[ReadPos % _slots.size()];

      for (
         auto seqN = 0u;
         (seqN = slot_Ptr->_seqN.load(std::memory_order_acquire)) != (ReadPos + 1u);)
      { // wait for slot "ready to be read"

         // no message waiting in this slot...

         if (_state.load(std::memory_order_acquire) == State_T::Closing)
         { // we want to close
            if (ReadPos == _writePos.load(std::memory_order_acquire))
            { // no more messages
               goto END_OF_CONSUMER_LABEL; // break out of both loops
            }
         }

         slot_Ptr->_seqN.wait(seqN, std::memory_order_acquire);
      }

      try
      { // attempt to write message to file
         fmt::print(_file.unwrap(), "{}", slot_Ptr->_msgBuffer.data());

         if (getOptions() == RedirectMode_T::ToLogAndStdOut)
         { // log it to console!
            ymLog(VF::Console, "{}", slot_Ptr->_msgBuffer.data());
         }
      }
      catch (std::exception const & E)
      { // logic or formatting error
         ymLog(VF::Errstream, "(global logger) fmt::print encountered an error. {}", E.what());
      }

      slot_Ptr->_seqN.store(ReadPos + _slots.size(), std::memory_order_release);
      slot_Ptr->_seqN.notify_all();
      _readPos.store(ReadPos + 1u, std::memory_order_relaxed);
   }

END_OF_CONSUMER_LABEL:
   return;
}

/*
 * Inner Class ScopedEnable functions.
 * -------------------------------------------------------------------------- */

/** ScopedEnable
 *
 * @brief Constructor.
 *
 * @note Enables upon construction.
 *
 * @param VF -- Verbosity flag.
 */
ym::GlobalLogger::ScopedEnable::ScopedEnable(VF const VFlag) :
   _VFlag      {VFlag},
   _WasEnabled {getGlobalInstance()->isVFlagEnabled(VFlag)}
{
   getGlobalInstance()->enable(_VFlag);
}

/** popEnable
 *
 * @brief Restores the enable state of the stored VF.
 *
 * @note This object could only have been created if getGlobalInstance() didn't throw, so we are
 *       safe in assuming this function won't throw either. Hence, the noexcept specifier.
 */
void ym::GlobalLogger::ScopedEnable::popEnable(void) const noexcept
{
   if (!_WasEnabled)
   { // disable
      getGlobalInstance()->disable(_VFlag);
   }
}
