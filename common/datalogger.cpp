/**
 * @file    datalogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "datalogger.h"

#include "textlogger.h"

#include "fmt/format.h"

#include <cstring>
#include <numeric>

/**
 * TODO
 */
ym::DataLogger::DataLogger(sizet const NTrackedValsHint) :
   _nTrackedValsHint {NTrackedValsHint}
{
   if (_nTrackedValsHint > 0uz)
   { // hint on how many tracked values - preallocate room for that many headers
      _trackedVals.reserve(_nTrackedValsHint);
      _nTrackedValsHint = _trackedVals.capacity(); // space it *actually* allocated
   }
}

/**
 * TODO
 */
bool ym::DataLogger::ready(sizet const MaxNEntries)
{
   YMASSERT(MaxNEntries > 0uz, Error, YM_DAH, "Depth of data logger must be > 0");

   if (_nTrackedValsHint > 0uz)
   { // supplied a hint
      if (_trackedVals.capacity() > _nTrackedValsHint)
      { // underestimated hint
         ymLog(VG::DataLogger, "Underestimated hint of {} - capacity is {}",
            _nTrackedValsHint, _trackedVals.capacity());
      }
   }

   auto const SumOfAllDataSizes_bytes = std::accumulate(
      _trackedVals.cbegin(),
      _trackedVals.cend(),
      0uz,
      [](sizet const Acc, RawTrackedVal_T const & RTV) {
         return Acc + RTV->_Size_bytes;
      }
   );

   auto const SizeOfBlackBoxBuffer_bytes = MaxNEntries * SumOfAllDataSizes_bytes;
   _blackBoxBuffer.resize(SizeOfBlackBoxBuffer_bytes);

   _nextEntry_idx = 0uz; // may no longer use _nTrackedValsHint (union)

   return true;
}

/** acquireAll
 *
 * @brief Reads all registered variables and stores them in the latest slot in the buffer.
 */
void ym::DataLogger::acquireAll(void)
{
   for (auto const & Val : _trackedVals)
   { // iterate through all registered values and read the associate variables
      std::memcpy(
         _blackBoxBuffer.data() + _nextEntry_idx, // dst
         Val->_Read_BPtr.get(),                   // src
         Val->_Size_bytes);
      _nextEntry_idx += Val->_Size_bytes;
   }

   _nextEntry_idx %= _blackBoxBuffer.size();

   if (_nextEntry_idx == 0uz)
   { // rollover happened
      _rollover = true;
   }
}

/** reset
 *
 * @brief Resets black box buffer.
 */
void ym::DataLogger::reset(void)
{
   _nextEntry_idx = 0uz;
   _rollover      = false;
}

/** dump
 *
 * @brief Dumps blackbox to file.
 * 
 * @param Filename -- Name of file to dump data to.
 *
 * TODO add dump to binary format mode
 * 
 * @returns bool -- If dump was successful.
 */
bool ym::DataLogger::dump(str const Filename)
{
   bool const Opened = openOutfile(Filename.get());

   if (Opened)
   { // file opened
      for (auto i = 0uz; i < _trackedVals.size(); i++)
      { // print all the headers
         if (i > 0uz)
         { // prevent printing trailing comma
            fmt::print(_outfile_uptr.get(), ",");
         }
         fmt::print(_outfile_uptr.get(), "{}", _trackedVals[i]->getName().get());
      }
      fmt::print(_outfile_uptr.get(), "\n");

      auto start_idx = 0uz;
      auto stop_idx  = _nextEntry_idx; // actual size of blackbox (not reserved size)

      if (_rollover)
      {
         start_idx = _nextEntry_idx;
         stop_idx  = start_idx + _blackBoxBuffer.size();
      }

      char buffer[100uz]{'\0'};

      for (auto i = start_idx; i < stop_idx; )
      { // print data from oldest to newest
         for (auto j = 0uz; j < _trackedVals.size(); j++)
         { // print row
            if (j > 0uz)
            { // prevent printing trailing comma
               fmt::print(_outfile_uptr.get(), ",");
            }

            _trackedVals[j]->toStr(_blackBoxBuffer.data() + _nextEntry_idx, buffer);
            _nextEntry_idx = _trackedVals[j]->_Size_bytes;
            fmt::print(_outfile_uptr.get(), "{}", buffer);
         }
         fmt::print(_outfile_uptr.get(), "\n");
      }
   }

   return Opened;
}

void ym::DataLogger::TrackedValBase::toStr_Handler(
   std::span<char>  buffer,
   fmt::format_args args) const
{
   if (!buffer.empty())
   { // room to write
      auto result = fmt::vformat_to_n(
         buffer.data(),
         buffer.size() - 1uz,
         "{}",
         args);
      *result.out = '\0';
   }
}
