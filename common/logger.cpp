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

   constexpr auto MaxFilenameSize_bytes = 256_u64;
   ymAssert<LoggerError>(FilenameSize_bytes < MaxFilenameSize_bytes,
      "Filename size is too big - not technically a hard error but unexpected. "
      "Got %lu bytes, max %lu bytes", FilenameSize_bytes, MaxFilenameSize_bytes);

   auto const TSFilenameSize_bytes =
      FilenameSize_bytes + s_DefaultTS.length() + 1_u64; // include null terminator
   auto * const tsFilename_Ptr = ymStackAlloc<char>(TSFilenameSize_bytes);

   // write stem
   std::strncpy(tsFilename_Ptr, Filename, StemSize_bytes);

   // write time stamp
   populateTimeStamp(tsFilename_Ptr + StemSize_bytes);

   // write extension
   std::strncpy(tsFilename_Ptr + StemSize_bytes + s_DefaultTS.length(),
                Filename + StemSize_bytes,
                FilenameSize_bytes - StemSize_bytes + 1ul); // include null terminator

   return openOutfile(tsFilename_Ptr);
}

/** populateTimeStamp
 *
 * @brief Writes the current timestamp to the specified buffer.
 *
 * @note If the current timestamp cannot be fetched 0's will be written instead.
 *
 * TODO link to s_DefaultTS
 *
 */
void ym::Logger::populateTimeStamp(char * const timeStamp_Ptr) const
{
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
                    s_DefaultTS.length() + 1_u64, // include null terminator
                    "_%Y_%b_%d_%H_%M_%S",
                    timeinfo_ptr);

   if (NBytesWritten != s_DefaultTS.length())
   { // failed to write time and contents of buffer are undefined
      std::strncpy(timeStamp_Ptr,
                   s_DefaultTS.data(),
                   s_DefaultTS.length() + 1_u64); // include null terminator
   }
}
