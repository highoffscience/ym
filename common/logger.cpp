/**
 * @author Forrest Jablonski
 */

#include "logger.h"

#include "memorypool.h"

#include <cstring>
#include <ctime>

/**
 *
 */
ym::Logger::Logger(void)
   : _outfile_uptr {nullptr, [](std::FILE * const Ptr) { std::fclose(Ptr); }}
{
}

/**
 *
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
   auto const StemSize_bytes = (Extension) ? (Extension - Filename) : FilenameSize_bytes;

   auto const TimeStampSize_bytes =
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

   auto const TimeStampedFilenameSize_bytes =
      FilenameSize_bytes + TimeStampSize_bytes + 1u; // +1 for null terminator

   // TODO should this return a smart pointer?
   auto * const timeStampedFilename_Ptr = MemoryPool<char>::allocate(TimeStampedFilenameSize_bytes);

   std::strncpy(timeStampedFilename_Ptr, Filename, StemSize_bytes);
   std::strncpy(timeStampedFilename_Ptr + StemSize_bytes + TimeStampSize_bytes,
                Filename + StemSize_bytes,
                FilenameSize_bytes - StemSize_bytes);

   auto t = std::time(nullptr);
   std::tm timeinfo = {0};
   auto * timeinfo_ptr = &timeinfo;
#if defined(_WIN32)
   localtime_s(timeinfo_ptr, &t); // vs doesn't have the standard localtime_s function
#else
   timeinfo_ptr = std::localtime(&t);
#endif // _WIN32

   auto const NBytesWritten =
      std::strftime(timeStampedFilename_Ptr + StemSize_bytes,
                    TimeStampSize_bytes,
                    "_%Y_%b_%d_%H_%M_%S.txt",
                    timeinfo_ptr);

   bool wasOpened = false;

   if (NBytesWritten > 0ul)
   {
      wasOpened = open(timeStampedFilename_Ptr);
   }
   else
   {
      printfError("Failure to store datetime!");
      wasOpened = false;
   }

   MemoryPool<char>::deallocate(timeStampedFilename_Ptr, TimeStampedFilenameSize_bytes);

   return wasOpened;
}
