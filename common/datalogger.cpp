/**
 * @file    datalogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "datalogger.h"

#include "textlogger.h"

#include "fmt/format.h"

#include <cstdio>
#include <cstring>
#include <numeric>

/** DataLogger
 * 
 * @brief Constructor. See ready().
 * 
 * @throws Error -- If requested depth is 0.
 * 
 * @param MaxDepth -- Max number of entries to store per tracked variable.
 */
ym::DataLogger::DataLogger(
   sizet const MaxDepth,
   sizet const NTrackedValsHint) :
      _MaxDepth         {MaxDepth        },
      _nTrackedValsHint {NTrackedValsHint}
{
   YMASSERT(getMaxDepth() > 0uz, Error, YM_DAH, "Depth of data logger must be > 0");

   if (_nTrackedValsHint > 0uz)
   { // hint on how many tracked values - preallocate room for that many headers
      _trackedVals.reserve(_nTrackedValsHint);
      _nTrackedValsHint = _trackedVals.capacity(); // space it *actually* allocated
   }
}

/** ready
 * 
 * @brief Initializes the data logger.
 * 
 * @throws Whatever std::vector::resize() throws.
 */
bool ym::DataLogger::ready(void)
{
   if (_nTrackedValsHint > 0uz)
   { // supplied a hint
      if (_trackedVals.capacity() > _nTrackedValsHint)
      { // underestimated hint
         ymLog(VG::DataLogger, "Underestimated hint of {} - capacity is {}",
            _nTrackedValsHint, _trackedVals.capacity());
      }
   }

   auto const SizeOfRow_bytes = std::accumulate(
      _trackedVals.cbegin(),
      _trackedVals.cend(),
      0uz,
      [](sizet const Acc, RawTrackedVal_T const & RTV) {
         return Acc + RTV->_Size_bytes;
      }
   );

   auto const SizeOfBlackBoxBuffer_bytes = getMaxDepth() * SizeOfRow_bytes;
   _blackBoxBuffer.resize(SizeOfBlackBoxBuffer_bytes);

   _nextEntry_idx = 0uz; // may no longer use _nTrackedValsHint (union)
   _initialized   = true;

   return _initialized;
}

/** acquire
 *
 * @brief Reads all registered variables and stores them in the latest slot in the buffer.
 * 
 * @throws Whatever ready() throws.
 */
void ym::DataLogger::acquire(void)
{
   if (!isInitialized())
   { // get the logger ready
      (void)ready();
   }

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
   _initialized   = false;
}

/** dump
 *
 * @brief Dumps blackbox to file.
 * 
 * @throws Error - If a logic error occurs.
 * 
 * @param Filename -- Name of file to dump data to.
 * @param Options  -- List of optional opening modes.
 * 
 * @returns bool -- If dump was successful.
 */
bool ym::DataLogger::dump(
   str       const   Filename,
   Options_T const & Options)
{
   bool const Opened = openOutfile(Filename.get(), Options);

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

      auto const SizeOfRow_bytes = _blackBoxBuffer.size() / getMaxDepth();
      YMASSERT(_nextEntry_idx % SizeOfRow_bytes == 0uz, Error, YM_DAH,
         "Data entry index {} expected to be a multiple of sum of entry sizes {}",
         _nextEntry_idx, SizeOfRow_bytes);

      auto nRowsCaptured = _nextEntry_idx / SizeOfRow_bytes;
      auto currEntry_idx = 0uz;

      if (_rollover)
      { // re-adjust start and stop indices
         nRowsCaptured = getMaxDepth();
         currEntry_idx = _nextEntry_idx;
      }

      if (Options == DumpMode_T::Binary)
      { // binary format
         if (_rollover)
         { // data not contiguous - requires two write blocks
            (void)std::fwrite(
               _blackBoxBuffer.data() + currEntry_idx,
               SizeOfRow_bytes,
               getMaxDepth() - (currEntry_idx / SizeOfRow_bytes), // # of rows from current entry to end
               _outfile_uptr.get());
            (void)std::fwrite(
               _blackBoxBuffer.data(),
               SizeOfRow_bytes,
               currEntry_idx / SizeOfRow_bytes, // # of rows from beginning to current entry
               _outfile_uptr.get());
         }
         else
         { // data contiguous - requires single write block
            (void)std::fwrite(_blackBoxBuffer.data(), SizeOfRow_bytes, nRowsCaptured, _outfile_uptr.get());
         }
      }
      else
      { // text format
         char buffer[100uz]{'\0'};
         for (auto i = 0uz; i < nRowsCaptured; i++)
         { // print data from oldest to newest
            for (auto j = 0uz; j < _trackedVals.size(); j++)
            { // print row
               if (j > 0uz)
               { // prevent printing trailing comma
                  fmt::print(_outfile_uptr.get(), ",");
               }
               _trackedVals[j]->toStr(_blackBoxBuffer.data() + currEntry_idx, buffer);
               currEntry_idx += _trackedVals[j]->_Size_bytes;
               fmt::print(_outfile_uptr.get(), "{}", buffer);
            }
            currEntry_idx %= _blackBoxBuffer.size();
            fmt::print(_outfile_uptr.get(), "\n");
         }
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
