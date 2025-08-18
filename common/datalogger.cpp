/**
 * @file    datalogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "datalogger.h"

#include "fmt/format.h"

#include <cstring>
#include <numeric>
#include <utility>

/**
 * TODO
 */
ym::DataLogger::DataLogger(sizet const HintNEntries)
{
   if (HintNEntries > 0uz)
   { // hint on how many entries this datalogger will hold
      // TODO issue warning if hint is used and datalogger has more entries than expected
      _columnHeaders.reserve(HintNEntries);
   }
}

/**
 * TODO
 */
bool ym::DataLogger::ready(
   sizet                           const MaxNEntries,
   bptr<std::pmr::memory_resource> const MemSrc)
{
   YMASSERT(MaxNEntries > 0uz, Error, YM_DAH, "Depth of data logger must be > 0");

   auto const SumOfAllDataSizes_bytes = std::accumulate(
      _columnHeaders.cbegin(),
      _columnHeaders.cend(),
      0uz,
      [](sizet const Acc, ColumnHeaderBase const & CHB) {
         return Acc + CHB._Size_bytes;
      }
   );

   auto const SizeOfBlackBoxBuffer_bytes = _columnHeaders.size() * SumOfAllDataSizes_bytes;
   auto newBlackBoxBuffer = decltype(_blackBoxBuffer)(MemSrc.get());
   newBlackBoxBuffer.reserve(SizeOfBlackBoxBuffer_bytes);
   _blackBoxBuffer = std::move(newBlackBoxBuffer);
}

/** acquireAll
 *
 * @brief Reads all registered variables and stores them in the latest slot in the buffer.
 */
void ym::DataLogger::acquireAll(void)
{
   // for (auto const & Entry : _columnEntries)
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
 * @brief Resets to initial empty state.
 */
void ym::DataLogger::reset(void)
{
   for (auto const & Entry : _columnEntries)
   { // iterates through all registered entries and clears the resources
      MemIO::dealloc<uint8>(Entry._dataEntries_Ptr, getMaxNDataEntries() * Entry._DataSize_bytes);
      delete Entry._Stringify_Ptr;
   }
   _columnEntries.clear();
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

void ym::DataLogger::IStringify::toStr_Handler(
   char * const     buffer_Ptr,
   fmt::format_args args) const
{
   auto result = fmt::vformat_to_n(
      buffer_Ptr,
      100,
      "{}",
      args);
   *result.out = '\0';
}
