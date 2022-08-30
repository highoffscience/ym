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
 * @param Filename
 *
 * @return bool -- true if the file was opened, false otherwise
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

   // _%Y_%b_%d_%H_%M_%S
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
      FilenameSize_bytes + TimeStampSize_bytes + 1ul; // +1 for null terminator

   // TODO I want this, I think
   // auto uptr = MemoryPool::getInstancePtr()->allocate<char>(TimeStampedFilenameSize_bytes);
   auto timeStampedFilename_uptr = MemoryPool<char>::allocate(TimeStampedFilenameSize_bytes);

   std::strncpy(timeStampedFilename_uptr.get(), Filename, StemSize_bytes);
   std::strncpy(timeStampedFilename_uptr.get() + StemSize_bytes + TimeStampSize_bytes,
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
      std::strftime(timeStampedFilename_uptr.get() + StemSize_bytes,
                    TimeStampSize_bytes,
                    "_%Y_%b_%d_%H_%M_%S",
                    timeinfo_ptr);

   /*
    * Attempting to open file now
    */

   bool opened = false;

   if (NBytesWritten > 0ul)
   {
      opened = openOutfile(timeStampedFilename_uptr.get());
   }
   else
   {
      printfError("Failure to store datetime!");
      opened = openOutfile(Filename);
   }

   return opened;
}
