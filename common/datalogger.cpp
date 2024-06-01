/**
 * @file    datalogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "datalogger.h"

#include <cstring>

/** DataLogger
 *
 * @brief Constructor.
 */
ym::DataLogger::DataLogger(uint64 const MaxNDataEntries)
   : _columnEntries     {/*default*/    },
     _MaxNDataEntries   {MaxNDataEntries},
     _nextDataEntry_idx {0_u64          },
     _rollover          {false          }
{
   DataLoggerError::check(_MaxNDataEntries > 0_u64, "Depth of data logger must be > 0");
}

/** ~DataLogger
 *
 * @brief Cleans up resources owned by the data logger.
 */
ym::DataLogger::~DataLogger(void)
{
   clear();
}

/** acquireAll
 *
 * @brief Reads all registered variables and stores them in the latest slot in the buffer.
 */
void ym::DataLogger::acquireAll(void)
{
   for (auto const & Entry : _columnEntries)
   { // iterates through all registered entries and reads the associate variables
      auto * const write_Ptr = Entry._dataEntries_Ptr + (Entry._DataSize_bytes * _nextDataEntry_idx);
      std::memcpy(write_Ptr, Entry._Read_Ptr, Entry._DataSize_bytes);
   }

   _nextDataEntry_idx = (_nextDataEntry_idx + 1_u64) % getMaxNDataEntries();

   if (_nextDataEntry_idx == 0_u64)
   { // rollover happened
      _rollover = true;
   }
}

/** clear
 *
 * @brief Cleans up resources owned by the data logger.
 */
void ym::DataLogger::clear(void)
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
 * @returns bool -- If dump was successful.
 */
bool ym::DataLogger::dump(str const Filename)
{
   bool const Opened = openOutfile(Filename);

   if (Opened)
   { // file opened
      for (auto j = 0_u64; j < _columnEntries.size(); ++j)
      { // print all the headers
         if (j > 0_u64)
         { // prevent printing trailing comma
            std::fprintf(_outfile_uptr.get(), ",");
         }
         auto const & Entry = _columnEntries[j];
         std::fprintf(_outfile_uptr.get(), "%s", Entry.getName().get());
      }
      std::fprintf(_outfile_uptr.get(), "\n");

      auto const Start_idx = _rollover ?
         (_nextDataEntry_idx + 1_u64) % getMaxNDataEntries() : 0_u64;

      for (auto i = Start_idx; i != _nextDataEntry_idx; i = (i + 1_u64) % getMaxNDataEntries())
      { // print data from oldest to newest
         for (auto j = 0_u64; j < _columnEntries.size(); ++j)
         { // print row
            if (j > 0_u64)
            { // prevent printing trailing comma
               std::fprintf(_outfile_uptr.get(), ",");
            }
            auto const & Entry = _columnEntries[j];
            void const * const Data_Ptr = Entry._dataEntries_Ptr + (Entry._DataSize_bytes * i);
            std::fprintf(_outfile_uptr.get(), "%s", Entry._Stringify_Ptr->toStr(Data_Ptr).c_str());
         }
         std::fprintf(_outfile_uptr.get(), "\n");
      }
   }

   return Opened;
}
