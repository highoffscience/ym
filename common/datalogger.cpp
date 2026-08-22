/**
 * @file    datalogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "datalogger.h"

#include "globallogger.h"

#include "fmt/format.h"

#include <cstdio>
#include <cstring>
#include <numeric>
#include <utility>

/**
 * @brief Constructor. See ready().
 *
 * @throws Error -- If requested depth is 0.
 *
 * @param MaxDepth -- Max number of entries to store per tracked variable.
 */
ym::DataLogger::DataLogger(
   std::size_t const MaxDepth,
   std::size_t const NTrackedValsHint) :
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

/**
 * @brief Initializes the data logger.
 */
bool ym::DataLogger::init(void) noexcept
{
   if (_nTrackedValsHint > 0uz)
   { // supplied a hint
      if (_trackedVals.capacity() > _nTrackedValsHint)
      { // underestimated hint
         ymLog(VF::DataLogger, "Underestimated hint of {} - capacity is {}",
            _nTrackedValsHint, _trackedVals.capacity());
      }
   }

   auto const SizeOfRow_bytes = std::accumulate(
      _trackedVals.cbegin(),
      _trackedVals.cend(),
      0uz,
      [](std::size_t const Acc, RawTrackedVal_T const & RTV) {
         return Acc + RTV->_Size_bytes;
      }
   );

   auto const SizeOfBlackBoxBuffer_bytes = getMaxDepth() * SizeOfRow_bytes;

   try
   { // attempt to allocate blackbox buffer
      _blackBoxBuffer.resize(SizeOfBlackBoxBuffer_bytes); // TODO if reset is called and this is called, what happens?

      _nextEntry_idx = 0uz; // can no longer use _nTrackedValsHint (union)
      _initialized   = true;
   }
   catch (std::exception const & E)
   { // couldn't allocate blackbox buffer
      ymLog(VF::Warning, "Couldn't allocate {} bytes for blackbox buffer. Exception - {}",
         SizeOfBlackBoxBuffer_bytes, E.what());

      _initialized = false;
   }

   return _initialized;
}

/**
 * @brief Reads all registered variables and stores them in the latest slot in the buffer.
 */
void ym::DataLogger::acquire(void) noexcept
{
   if (!isInitialized())
   { // get the logger ready
      if (!init())
      { // something went wrong
         ymLog(VF::Warning, "DataLogger failed to initialize!");
         return;
      }
   }

   for (auto const & Val : _trackedVals)
   { // iterate through all registered values and read the associate variables
      std::ignore = std::memcpy(
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

/**
 * @brief Resets black box buffer.
 */
void ym::DataLogger::reset(void) noexcept
{
   _nextEntry_idx = 0uz;
   _rollover      = false;
   _initialized   = false;
}

/**
 * @brief Dumps blackbox to file.
 *
 * @throws Error -- If entry index is not of expected value.
 *
 * @param Filename -- Name of file to dump data to.
 *
 * @returns bool -- If dump was successful.
 */
bool ym::DataLogger::dump(strlit const Filename)
{
   bool const Opened = openOutfile(Filename.get());

   if (auto * const outfile_Ptr = _file.unwrap().get(); Opened)
   { // file opened -> not null
      for (auto i = 0uz; i < _trackedVals.size(); i++)
      { // print all the headers
         if (i > 0uz)
         { // prevent printing trailing comma
            fmt::print(outfile_Ptr, ",");
         }
         fmt::print(outfile_Ptr, "{}", _trackedVals[i]->getName().get());
      }
      fmt::print(outfile_Ptr, "\n");

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

      if (getOptions() == DumpMode_T::Binary)
      { // binary format
         if (_rollover)
         { // data not contiguous - requires two write blocks
            std::ignore = std::fwrite(
               _blackBoxBuffer.data() + currEntry_idx,
               SizeOfRow_bytes,
               getMaxDepth() - (currEntry_idx / SizeOfRow_bytes), // # of rows from current entry to end
               outfile_Ptr);
            std::ignore = std::fwrite(
               _blackBoxBuffer.data(),
               SizeOfRow_bytes,
               currEntry_idx / SizeOfRow_bytes, // # of rows from beginning to current entry
               outfile_Ptr);
         }
         else
         { // data contiguous - requires single write block
            std::ignore = std::fwrite(_blackBoxBuffer.data(), SizeOfRow_bytes, nRowsCaptured, outfile_Ptr);
         }
      }
      else
      { // text format
         char buffer[64uz]{'\0'};
         for (auto i = 0uz; i < nRowsCaptured; i++)
         { // print data from oldest to newest
            for (auto j = 0uz; j < _trackedVals.size(); j++)
            { // print row
               if (j > 0uz)
               { // prevent printing trailing comma
                  fmt::print(outfile_Ptr, ",");
               }
               _trackedVals[j]->toStr(_blackBoxBuffer.data() + currEntry_idx, buffer);
               currEntry_idx += _trackedVals[j]->_Size_bytes;
               fmt::print(outfile_Ptr, "{}", buffer);
            }
            currEntry_idx %= _blackBoxBuffer.size();
            fmt::print(outfile_Ptr, "\n");
         }
      }
   }

   return Opened;
}

/**
 * @brief Writes arguments to the supplied buffer.
 *
 * @param buffer -- Output buffer.
 * @param args   -- Data to write.
 */
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
