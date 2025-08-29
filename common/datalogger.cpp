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

   // auto const SumOfAllDataSizes_bytes = std::accumulate(
   //    _trackedVals.cbegin(),
   //    _trackedVals.cend(),
   //    0uz,
   //    [](sizet const Acc, TrackedValBase const & TVB) {
   //       return Acc + TVB._Size_bytes;
   //    }
   // );

   // auto const SizeOfBlackBoxBuffer_bytes = _trackedVals.size() * SumOfAllDataSizes_bytes;
   // _blackBoxBuffer.reserve(SizeOfBlackBoxBuffer_bytes);

   _nextEntry_idx = 0uz; // may no longer use _nTrackedValsHint (union)

   return true;
}

/** acquireAll
 *
 * @brief Reads all registered variables and stores them in the latest slot in the buffer.
 */
void ym::DataLogger::acquireAll(void)
{
   // for (auto const & Val : _trackedVals)
   // { // iterates through all registered entries and reads the associate variables
   //    auto * const write_Ptr = Entry._dataEntries_Ptr + (Entry._DataSize_bytes * _nextDataEntry_idx);
   //    std::memcpy(write_Ptr, Entry._Read_Ptr, Entry._DataSize_bytes);
   // }

   // _nextDataEntry_idx = (_nextDataEntry_idx + 1_u64) % getMaxNDataEntries();

   // if (_nextDataEntry_idx == 0_u64)
   // { // rollover happened
   //    _rollover = true;
   // }
}

/** reset
 *
 * @brief Resets black box buffer.
 */
void ym::DataLogger::reset(void)
{
   _blackBoxBuffer.clear();
   _nextEntry_idx = 0uz;
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
   (void)Filename;
   // bool const Opened = openOutfile(Filename);

   // if (Opened)
   // { // file opened
   //    for (auto j = 0_u64; j < _columnEntries.size(); ++j)
   //    { // print all the headers
   //       if (j > 0_u64)
   //       { // prevent printing trailing comma
   //          fmt::fprintf(_outfile_uptr.get(), ",");
   //       }
   //       auto const & Entry = _columnEntries[j];
   //       fmt::fprintf(_outfile_uptr.get(), "{}", Entry.getName().get());
   //    }
   //    fmt::fprintf(_outfile_uptr.get(), "\n");

   //    auto const Start_idx = _rollover ?
   //       (_nextDataEntry_idx + 1_u64) % getMaxNDataEntries() : 0_u64;

   //    for (auto i = Start_idx; i != _nextDataEntry_idx; i = (i + 1_u64) % getMaxNDataEntries())
   //    { // print data from oldest to newest
   //       for (auto j = 0_u64; j < _columnEntries.size(); ++j)
   //       { // print row
   //          if (j > 0_u64)
   //          { // prevent printing trailing comma
   //             fmt::fprintf(_outfile_uptr.get(), ",");
   //          }
   //          auto const & Entry = _columnEntries[j];
   //          void const * const Data_Ptr = Entry._dataEntries_Ptr + (Entry._DataSize_bytes * i);
   //          fmt::fprintf(_outfile_uptr.get(), "{}", Entry._Stringify_Ptr->toStr(Data_Ptr).c_str());
   //       }
   //       fmt::fprintf(_outfile_uptr.get(), "\n");
   //    }
   // }

   // return Opened;
   return false;
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
