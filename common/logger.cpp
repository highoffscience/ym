/**
 * @file    logger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "logger.h"

#include "memorypool.h"

#include <cstring>
#include <ctime>

/**
 * Constructor.
 *
 * _outfile_uptr is set to null to serve as a flag that the logger is uninitialized.
 */
ym::Logger::Logger(void)
   : _outfile_uptr {nullptr, [](std::FILE * const file_Ptr) { std::fclose(file_Ptr); }}
{
}

/**
 * We open the file here instead of the constructor to allow flexibility with
 * derived classes handling the file operations.
 *
 * The conditional assures the logger is only associated with one file.
 *
 * @param Filename -- Name of file to open.
 *
 * @return bool -- True if the file was opened, false otherwise.
 */
bool ym::Logger::openOutfile(str const Filename)
{
   if (!_outfile_uptr)
   {
      _outfile_uptr.reset(std::fopen(Filename, "w"));
   }

   return static_cast<bool>(_outfile_uptr);
}

/**
 *
 */
bool ym::Logger::openOutfile_appendTimeStamp(str const Filename)
{
   auto const FilenameSize_bytes = std::strlen(Filename);
   auto const Extension = std::strchr(Filename, '.');

   // Extension pointer is guaranteed to be at least as large as Filename pointer
   auto const StemSize_bytes = (Extension) ? (Extension - Filename) : FilenameSize_bytes;

   // TS = Time Stamp

   // _%Y_%b_%d_%H_%M_%S
   constexpr auto TSSize_bytes =
      1u + // _
      4u + // year
      1u + // _
      3u + // month
      1u + // _
      2u + // day
      1u + // _
      2u + // hour
      1u + // _
      2u + // minute
      1u + // _
      2u;  // second

   auto const TSFilenameSize_bytes = FilenameSize_bytes + TSSize_bytes + 1ul; // +1 for null terminator

   // TODO I want this, I think
   // auto uptr = MemoryPool::getInstancePtr()->allocate<char>(TimeStampedFilenameSize_bytes);
   auto tsFilename_uptr = MemoryPool<char>::allocate(TSFilenameSize_bytes);

   // write stem
   std::strncpy(tsFilename_uptr.get(), Filename, StemSize_bytes + 1u); // include null terminator for safety

   { // getting current time
      auto t = std::time(nullptr);
      std::tm timeinfo = {0};
      auto * timeinfo_ptr = &timeinfo;
   #if defined(_WIN32)
      localtime_s(timeinfo_ptr, &t); // vs doesn't have the standard localtime_s function
   #else
      timeinfo_ptr = std::localtime(&t);
   #endif // _WIN32

      auto const NBytesWritten =
         // write time stamp
         std::strftime(tsFilename_uptr.get() + StemSize_bytes,
                       TSSize_bytes + 1u, // needs room for null terminator
                       "_%Y_%b_%d_%H_%M_%S",
                       timeinfo_ptr);

      if (NBytesWritten != TSSize_bytes)
      {
         std::strncpy(tsFilename_uptr.get() + StemSize_bytes,
                      "_0000_000_00_00_00_00",
                      TSSize_bytes + 1u); // include null terminator for safety
      }
   }

   // write extension
   std::strncpy(tsFilename_uptr.get() + StemSize_bytes + TSSize_bytes,
                Filename + StemSize_bytes,
                FilenameSize_bytes - StemSize_bytes + 1ul); // include null terminator

   /*
    * Attempting to open file now
    */

   return openOutfile(tsFilename_uptr.get());
}
