/**
 * @file    logger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "logger.h"

#include "memio.h"

#include <cstring>
#include <ctime>
#include <filesystem>

/** Logger
 *
 * @brief Constructor.
 *
 * @note _outfile_uptr is set to null to serve as a flag that the logger is uninitialized.
 */
ym::Logger::Logger(void)
   : _outfile_uptr {nullptr,
      [](std::FILE * const file_Ptr) {
         if (file_Ptr != stdout &&
             file_Ptr != stderr) {
            (void)std::fclose(file_Ptr);
         }
      }
   }
{
}

/** openOutfile
 *
 * @brief Attempts to open a write-file.
 *
 * @note We open the file here instead of the constructor to allow flexibility with
 *       derived classes handling the file operations. Also it's awkward to code
 *       constructors that throw.
 *
 * @note The conditional assures the logger is only associated with one file.
 * 
 * @throws LoggerError_FailureToOpen -- If the file already exists.
 * @throws LoggerError_FailureToOpen -- If filesystem utility function fails.
 *
 * @param Filename     -- Name of file to open.
 * @param FilenameMode -- Mode whether to append time stamps to filename.
 *
 * @returns bool -- True if the file was opened, false otherwise.
 */
bool ym::Logger::openOutfile(str            const Filename,
                             FilenameMode_T const FilenameMode)
{
   // don't rely on isOutfileOpened(), since we want to return if the file
   // was opened, which is a no if the file already exists, but isOutfileOpened()
   // would return true - not the behaviour we desire
   auto opened = false; // until told otherwise

   if (!isOutfileOpened())
   { // file not opened
      if (ymIsStrNonEmpty(Filename))
      { // valid filename specified
         if (FilenameMode == FilenameMode_T::AppendTimeStamp)
         { // append file stamp
            opened = openOutfile_appendTimeStamp(Filename);
         }
         else
         { // do not append file stamp (default fallthrough)
            try
            { // to catch utility failures
               LoggerError_FailureToOpen::check(!std::filesystem::exists(Filename.get()),
                  "File (or directory) %s already exists", Filename.get());
            }
            catch (Ymception const & E)
            { // re-throw
               throw;
            }
            catch (std::exception const & E)
            { // filesystem utility failure
               LoggerError_FailureToOpen::check(false,
                  "Filesystem error when attempting to open %s (%s)", Filename.get(), E.what());
            }
            
            _outfile_uptr.reset(std::fopen(Filename, "w"));
            opened = isOutfileOpened();
         }
      }
   }

   return opened;
}

/** openOutfile
 * 
 * @brief Attempts to open a write-file from an existing stream.
 *
 * @note We open the file here instead of the constructor to allow flexibility with
 *       derived classes handling the file operations. Also it's awkward to code
 *       constructors that throw.
 *
 * @note The conditional assures the logger is only associated with one file.
 * 
 * @throws LoggerError_FailureToOpen -- If the file already exists.
 *
 * @param file_Ptr -- Existing file stream.
 *
 * @returns bool -- True if the file was opened, false otherwise.
 */
bool ym::Logger::openOutfile(std::FILE * const file_Ptr)
{
   // don't rely on isOutfileOpened(), since we want to return if the file
   // was opened, which is a no if the file already exists, but isOutfileOpened()
   // would return true - not the behaviour we desire
   auto opened = false; // until told otherwise

   if (!isOutfileOpened())
   { // file not opened
      LoggerError_FailureToOpen::check(file_Ptr, "File ptr is not valid");

      _outfile_uptr.reset(file_Ptr);
      opened = isOutfileOpened();
   }

   return opened;
}

/** openOutfile_appendTimeStamp
 *
 * @brief Attempts to open the file with the current time appended to the file name.
 * 
 * @note Possibly use @ref <https://en.cppreference.com/w/cpp/filesystem/path>.
 * 
 * @throws LoggerError_FailureToOpen -- If the filename size is unexpectedly large.
 * 
 * @param Filename -- Name of file.
 *
 * @returns bool -- True if file was opened, false otherwise.
 */
bool ym::Logger::openOutfile_appendTimeStamp(str const Filename)
{
   auto const FilenameSize_bytes = std::strlen(Filename);
   auto const Extension          = std::strchr(Filename, '.');
   auto const StemSize_bytes     = (Extension) ? (Extension - Filename) // Extension >= Filename
                                               :  FilenameSize_bytes;

   constexpr auto MaxFilenameSize_bytes = 256_u64;
   LoggerError_FailureToOpen::check(FilenameSize_bytes < MaxFilenameSize_bytes,
      "Filename size is too big - not technically a hard error but too unexpected. "
      "Got %lu bytes, max %lu bytes", FilenameSize_bytes, MaxFilenameSize_bytes);

   auto const TSFilenameSize_bytes =
      FilenameSize_bytes + s_DefaultTS.length() + 1_u64; // include null terminator
   auto * const tsFilename_Ptr = YM_STACK_ALLOC(char, TSFilenameSize_bytes);

   // write stem
   std::strncpy(tsFilename_Ptr, Filename, StemSize_bytes);

   // write time stamp
   populateFilenameTimeStamp(tsFilename_Ptr + StemSize_bytes);

   // write extension
   std::strncpy(tsFilename_Ptr + StemSize_bytes + s_DefaultTS.length(),
                Filename + StemSize_bytes,
                FilenameSize_bytes - StemSize_bytes + 1ul); // include null terminator

   return openOutfile(tsFilename_Ptr, FilenameMode_T::KeepOriginal);
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
