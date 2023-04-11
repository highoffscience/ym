/**
 * @file    logger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "logger.h"

#include "memio.h"

#include <cstring>
#include <ctime>

/** Logger
 *
 * @brief Constructor.
 *
 * @note _outfile_uptr is set to null to serve as a flag that the logger is uninitialized.
 */
ym::Logger::Logger(void)
   : _outfile_uptr {nullptr, [](std::FILE * const file_Ptr) { if (file_Ptr != stdout) { std::fclose(file_Ptr); } }}
{
}

/** openOutfileToStdout
 *
 * @brief Opens outfile to standard out.
 * 
 * @return bool -- Tru if the outfile was successfully opened, false otherwise.
 */
bool ym::Logger::openOutfileToStdout(void)
{
   _outfile_uptr.reset(stdout);
   return static_cast<bool>(_outfile_uptr);
}

/** openOutfile
 *
 * @brief Attempts to open a write-file.
 *
 * @param Filename -- Name of file to open
 *
 * @return bool -- True if the file was opened, false otherwise
 */
bool ym::Logger::openOutfile(str const Filename)
{
   return openOutfile(Filename, TimeStampFilenameMode_T::Append);
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
 * @param Filename       -- Name of file to open.
 * @param TSFilenameMode -- Mode whether to append time stamps to filename.
 *
 * @return bool -- True if the file was opened, false otherwise
 */
bool ym::Logger::openOutfile(str    const Filename,
                             TSFM_T const TSFilenameMode)
{
   auto opened = false; // until told otherwise

   if (!isOutfileOpened())
   { // file not opened
      if (Filename && *Filename != '\0')
      { // valid filename specified
         if (TSFilenameMode == TimeStampFilenameMode_T::Append)
         { // append file stamp
            opened = openOutfile_appendTimeStamp(Filename);
         }
         else
         { // do not append file stamp (default fallthrough)
            _outfile_uptr.reset(std::fopen(Filename, "w"));
            opened = static_cast<bool>(_outfile_uptr);
         }
      }
   }

   return opened;
}  

/** openOutfile_appendTimeStamp
 *
 * @brief Attempts to open the file with the current time appended to the file name.
 *
 * @param Filename -- Name of file.
 *
 * @return bool -- True if file was opened, false otherwise.
 */
bool ym::Logger::openOutfile_appendTimeStamp(str const Filename)
{
   auto const FilenameSize_bytes = std::strlen(Filename);
   auto const Extension          = std::strchr(Filename, '.');
   auto const StemSize_bytes     = (Extension) ? (Extension - Filename) // Extension >= Filename
                                               :  FilenameSize_bytes;

   // constexpr auto MaxFilenameSize_bytes = 256_u64;
   // ymAssert_Helper<LoggerError_UnexpectedFilenameSize>(FilenameSize_bytes < MaxFilenameSize_bytes,
   //    "Filename size is too big - not technically a hard error but unexpected. "
   //    "Got {} bytes, max {} bytes", FilenameSize_bytes, MaxFilenameSize_bytes);

   auto const TSFilenameSize_bytes =
      FilenameSize_bytes + s_DefaultTS.length() + 1_u64; // include null terminator
   auto * const tsFilename_Ptr = YM_STACK_ALLOC(char, TSFilenameSize_bytes);
   //auto * const tsFilename_Ptr = static_cast<char *>(alloca(TSFilenameSize_bytes * sizeof(char)));

   // write stem
   std::strncpy(tsFilename_Ptr, Filename, StemSize_bytes);

   // write time stamp
   populateFilenameTimeStamp(tsFilename_Ptr + StemSize_bytes);

   // write extension
   std::strncpy(tsFilename_Ptr + StemSize_bytes + s_DefaultTS.length(),
                Filename + StemSize_bytes,
                FilenameSize_bytes - StemSize_bytes + 1ul); // include null terminator

   return openOutfile(tsFilename_Ptr, TimeStampFilenameMode_T::DoNotAppend);
}

/** closeOutfile
 *
 * @brief Closes the file and disassociates the file handle.
 */
void ym::Logger::closeOutfile(void)
{
   _outfile_uptr.reset(nullptr);
}

/** populateFilenameTimeStamp
 *
 * @brief Writes the current timestamp to the specified buffer.
 *
 * @note If the current timestamp cannot be fetched 0's will be written instead.
 *
 * @link Logger::s_DefaultTS @endlink
 *
 * @param timeStamp_Ptr -- Pointer to buffer to write timestamp to
 */
void ym::Logger::populateFilenameTimeStamp(char * const timeStamp_Ptr) const
{
   auto t = std::time(nullptr);
   std::tm timeinfo;
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
