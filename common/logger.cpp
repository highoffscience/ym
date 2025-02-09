/**
 * @file    logger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "logger.h"

// TODO
// #include "memio.h"
// #include "textlogger.h"
#include "timer.h"
#include "ymassert.h"

#include "fmt/core.h"

#include <cstring>
#include <ctime>
#include <filesystem>
#include <stdexcept>
#include <string>

/** Logger
 *
 * @brief Constructor.
 *
 * @note _outfile_uptr is set to null to serve as a flag that the logger is uninitialized.
 */
ym::Logger::Logger(void)
   : _outfile_uptr {nullptr,
      [] (std::FILE * const file_Ptr) {
         if (file_Ptr != stdout &&
             file_Ptr != stderr) { // don't try to close standard streams
            [[maybe_unused]] auto const RetVal = std::fclose(file_Ptr);
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
 * @param Filename     -- Name of file to open.
 * @param FilenameMode -- Mode whether to append time stamps to filename.
 *
 * @returns bool -- True if the file was opened, false otherwise.
 */
bool ym::Logger::openOutfile(
   rawstr         const Filename,
   FilenameMode_T const FilenameMode)
{
   // don't rely on isOutfileOpened(), since we want to return if the file
   // was opened, which is a no if the file already exists, but isOutfileOpened()
   // would return true - not the behaviour we desire
   auto opened = false; // until told otherwise

   if (!isOutfileOpened())
   { // file not opened
      if (Filename && *Filename)
      { // valid filename specified
         if (FilenameMode == FilenameMode_T::AppendTimeStamp)
         { // append file stamp
            opened = openOutfile_appendTimeStamp(Filename);
         }
         else
         { // do not append file stamp (default fallthrough)
            try
            { // to catch utility failures
               if (std::filesystem::exists(Filename))
               {
                  // TODO
                  // ymLog("Warning: File (or directory) {} already exists", Filename);
               }
            }
            catch (std::exception const & E)
            { // filesystem utility failure
               // TODO
               // ymLog("Warning: Filesystem error when attempting to open %s (%s)", Filename, E.what());
            }
            
            _outfile_uptr.reset(std::fopen(Filename, "w"));
            opened = isOutfileOpened();
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
 * @returns bool -- True if file was opened, false otherwise.
 */
bool ym::Logger::openOutfile_appendTimeStamp(std::string_view const Filename)
{
   auto extPos = Filename.find_last_of('.');
   if (extPos == 0_u64 ||                // hidden files
       extPos == std::string_view::npos) // no extension
   { // no extension found
      extPos = Filename.size();
   }

   std::string_view ext;
   try
   {
      ext = Filename.substr(extPos);
   }
   catch (std::out_of_range const & E)
   {
      // TODO
      // YMASSERT_CATCHALL(false, "{}", E.what());
      return false;
   }

   constexpr std::string_view TimeStamp("_YYYY_mm_dd_HH_MM_SS");

   // TODO use custom allocator
   std::string timeStampedFilename(Filename.size() + TimeStamp.size(), '\0');

   // write stem
   auto result = fmt::format_to_n(
      timeStampedFilename.data(),
      Filename.size() - ext.size(),
      "{}",
      Filename);

   // write time stamp
   result = fmt::format_to_n(
      result.out,
      TimeStamp.size(),
      "_{:%Y_%m_%d_%H_%M_%S}",
      Timer::Clock_T::now());

   // write extension
   result = fmt::format_to_n(
      result.out,
      ext.size(),
      "{}",
      ext);

   *result.out = '\0';

   return openOutfile(timeStampedFilename.c_str(), FilenameMode_T::KeepOriginal);
}

/** closeOutfile
 *
 * @brief Closes the file and disassociates the file handle.
 */
void ym::Logger::closeOutfile(void)
{
   _outfile_uptr.reset(nullptr);
}
