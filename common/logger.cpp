/**
 * @file    logger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "logger.h"

#include "textlogger.h"
#include "timer.h"

#include <array>
#include <exception>
#include <filesystem>
#include <format>
#include <memory_resource>
#include <string>
#include <system_error>

/** Logger
 *
 * @brief Constructor.
 * 
 * @param Filename     -- Name of file to open.
 * @param FilenameMode -- Mode whether to append time stamps to filename.
 *
 * @note _outfile_uptr is set to null to serve as a flag that the logger is uninitialized.
 */
ym::Logger::Logger(
   str            const Filename,
   FilenameMode_T const FilenameMode) :
      _outfile_uptr {nullptr,
         [] (std::FILE * const file_Ptr) {
            if (file_Ptr != stdout &&
               file_Ptr != stderr) { // don't try to close standard streams
               [[maybe_unused]]
               auto const RetVal = std::fclose(file_Ptr);
            }
         }
      },
      _Filename     {Filename    },
      _FilenameMode {FilenameMode}
{ }

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
 * @returns bool -- True if the file was opened, false otherwise.
 */
bool ym::Logger::openOutfile(void)
{
   // don't rely on isOutfileOpened(), since we want to return if the file
   // was opened, which is a no if the file already exists, but isOutfileOpened()
   // would return true - not the behaviour we desire

   auto opened = false; // until told otherwise

   if (!isOutfileOpened())
   { // file not opened
      if (getFilenameMode() == FilenameMode_T::AppendTimeStamp)
      { // append file stamp
         opened = openOutfile_appendTimeStamp(getFilename().get());
      }
      else
      { // do not append file stamp (default fallthrough)
         opened = openOutfile(getFilename().get());
      }
   }

   return opened;
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
 * @param Filename -- Name of file.
 *
 * @returns bool -- True if the file was opened, false otherwise.
 */
bool ym::Logger::openOutfile(std::string_view const Filename)
{
   auto opened = false; // until told otherwise

   std::error_code ec;
   if (std::filesystem::exists(Filename, ec))
   { // file we are attempting to create already exists
      ymLog(VG::Logger, "WARNING: File (or directory) '{}' already exists", Filename);
   }
   else if (ec)
   { // filesystem failure 
      ymLog(VG::Logger, "WARNING: Filesystem error when attempting to open '{}' with error code {}", Filename, ec.value());
   }
   else
   { // open!

      _outfile_uptr.reset(std::fopen(Filename.data(), "w"));
      opened = isOutfileOpened();
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
   if (extPos == std::size_t(0u) ||      // hidden files
       extPos == std::string_view::npos) // no extension
   { // no extension found
      extPos = Filename.size();
   }

   std::string_view ext;
   try
   { // get extension
      ext = Filename.substr(extPos);
   }
   catch (std::out_of_range const & E)
   { // logic error
      YMASSERT(false, OpenError, YM_DAH, "Error finding extension. {}", E.what())
   }

   constexpr std::string_view TimeStamp("_YYYY_mm_dd_HH_MM_SS");

   std::array<char, 256u> buffer{'\0'};
   std::pmr::monotonic_buffer_resource mbr{buffer.data(), buffer.size(), std::pmr::null_memory_resource()};
   std::pmr::string timeStampedFilename(Filename.size() + TimeStamp.size(), '\0', &mbr);

   // write stem
   auto result = std::format_to_n(
      timeStampedFilename.data(),
      Filename.size() - ext.size(),
      "{}",
      Filename);

   // write time stamp
   result = std::format_to_n(
      result.out,
      TimeStamp.size(),
      "_{:%Y_%m_%d_%H_%M_%S}",
      Timer::Clock_T::now());

   // write extension
   result = std::format_to_n(
      result.out,
      ext.size(),
      "{}",
      ext);

   YMASSERT(result.out == &*timeStampedFilename.end(), OpenError, YM_DAH,
      "Error printing time stamp. {} -- {}", (void*)result.out, (void*)&*timeStampedFilename.end())

   return openOutfile(timeStampedFilename);
}

/** closeOutfile
 *
 * @brief Closes the file and disassociates the file handle.
 */
void ym::Logger::closeOutfile(void)
{
   _outfile_uptr.reset(nullptr);
}
