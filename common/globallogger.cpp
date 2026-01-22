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
   for (auto i = 0u; i < _slots.size(); i++)
   { // init slot sequence numbers
      _slots[i]._seqN.store(i, std::memory_order_relaxed);
   }
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
      auto const Opened = openOutfile(getFilename().get());
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
   if (auto expectedState = State_T::Open; _state.compare_exchange_strong(
      expectedState, State_T::Closing,
      std::memory_order_acquire,
      std::memory_order_relaxed))
   { // file opened - let's change that
      closeOutfile();
      _state.store(State_T::Closed, std::memory_order_relaxed); // TODO release?
   }
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

   // YMASSERT(result.out >= buffer, Error,
   //    [this](auto const & E) -> void {
   //       this->releaseWriteAccess();
   //       throw E;
   //    }, "Format to buffer did not behave as expected")

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
 * @brief TODO consumer
 */
void ym::GlobalLogger::printer(void)
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
               _state.store(State_T::Closed, std::memory_order_release);
               break;
            }
         }

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
}
