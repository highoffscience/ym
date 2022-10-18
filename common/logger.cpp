/**
 * @file    logger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "logger.h"

#include "ymception.h"

#include <cstring>
#include <ctime>

/** Logger
 *
 * @brief Constructor.
 *
 * @note _outfile_uptr is set to null to serve as a flag that the logger is uninitialized.
 */
ym::Logger::Logger(void)
   : _outfile_uptr {nullptr, [](std::FILE * const file_Ptr) { std::fclose(file_Ptr); }}
{
}

/** openOutfile
 *
 * @brief Attempts to open a write-file.
 *
 * @note We open the file here instead of the constructor to allow flexibility with
 *       derived classes handling the file operations.
 *
 * @note The conditional assures the logger is only associated with one file.
 *
 * @param Filename -- Name of file to open
 *
 * @return bool -- True if the file was opened, false otherwise
 */
bool ym::Logger::openOutfile(str const Filename)
{
   if (!_outfile_uptr)
   {
      _outfile_uptr.reset(std::fopen(Filename, "w"));
   }

   return static_cast<bool>(_outfile_uptr);
}

/** openOutfile_appendTimeStamp
 *
 * @brief Attempts to open the file with the current time appended to the file name.
 *
 * @param Filename -- Name of file
 *
 * @return bool -- True if file was opened, false otherwise
 */
bool ym::Logger::openOutfile_appendTimeStamp(str const Filename)
{
   auto const FilenameSize_bytes = std::strlen(Filename);
   auto const Extension          = std::strchr(Filename, '.');
   auto const StemSize_bytes     = (Extension) ? (Extension - Filename) // Extension >= Filename
                                               :  FilenameSize_bytes;

   // TS = Time Stamp(ed)

   constexpr auto MaxFilenameSize_bytes = 256_u64;
   ymAssert<LoggerError>(FilenameSize_bytes < MaxFilenameSize_bytes,
      "Filename size is too big - not technically a hard error but unexpected. "
      "Got %lu bytes, max %lu bytes", FilenameSize_bytes, MaxFilenameSize_bytes);

   // TODO @link to populateTimeStamp()
   //                                            _%Y  _%b_ %d_%H_%M_%S
   constexpr auto TSSize_bytes         = sizeof("_0000_000_00_00_00_00"); // includes null terminator
   auto const     TSFilenameSize_bytes = FilenameSize_bytes + TSSize_bytes;

   auto * tsFilename = ymStackAlloc<char>(TSFilenameSize_bytes);

   // write stem
   std::strncpy(tsFilename, Filename, StemSize_bytes); // do not include null terminator

   // write time stamp
   populateTimeStamp(tsFilename + StemSize_bytes);

   // write extension
   std::strncpy(tsFilename + StemSize_bytes + TSSize_bytes,
                  Filename + StemSize_bytes,
                  FilenameSize_bytes - StemSize_bytes + 1ul); // include null terminator

   return openOutfile(tsFilename);
}

/**
 *
 */
void ym::Logger::populateTimeStamp(char * const timeStamp_Ptr) const
{
   constexpr auto * EmptyTS      =        "_0000_000_00_00_00_00";
   constexpr auto   TSSize_bytes = sizeof("_0000_000_00_00_00_00"); // sizeof EmptyTS = 8

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
      std::strftime(timeStamp_Ptr,
                    TSSize_bytes, // null terminator is included in this size
                    "_%Y_%b_%d_%H_%M_%S",
                    timeinfo_ptr);

   if (NBytesWritten != TSSize_bytes)
   { // failed to write time and contents of buffer are undefined
      std::strncpy(timeStamp_Ptr, EmptyTS, TSSize_bytes);
   }
}
