/**
 * @file    logger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "logger.h"

#include "globallogger.h"
#include "timer.h"

#include "fmt/chrono.h"
#include "fmt/format.h"

#include <array>
#include <exception>
#include <filesystem>
#include <memory_resource>
#include <string>
#include <system_error>
#include <utility>

/** ~Logger
 * 
 * @brief Destructor.
 */
ym::Logger::~Logger(void) noexcept
{
   if (isOutfileOpened())
   { // file still opened - time to close it
      closeOutfile();
   }
}

/** openOutfile
 *
 * @brief Attempts to open a write-file.
 * 
 * @param Filename -- Name of file.
 * 
 * @note We open the file here instead of the constructor to allow flexibility with
 *       derived classes handling the file operations. Also it's awkward to code
 *       constructors that throw.
 *
 * @returns bool -- True if the file is/was opened, false otherwise.
 */
bool ym::Logger::openOutfile(std::string_view const Filename) noexcept
{
   if (!isOutfileOpened())
   { // file not opened
      if (getOptions() == FilenameMode_T::AppendTimeStamp)
      { // append file stamp
         openOutfile_appendTimeStamp(Filename);
      }
      else
      { // do not append file stamp (default fallthrough)
         openOutfile_core(Filename);
      }
   }

   return isOutfileOpened();
}

/** closeOutfile
 * 
 * @brief Closes file if open.
 */
void ym::Logger::closeOutfile(void) noexcept
{
   if (isOutfileOpened())
   { // something to close
      std::ignore = std::fclose(_file.unwrap());
      _file = nullptr;
   }
}

/** openOutfile_core
 *
 * @brief Attempts to open a write-file.
 * 
 * @param Filename -- Name of file.
 */
void ym::Logger::openOutfile_core(std::string_view const Filename) noexcept
{
   std::error_code ec;
   if (getOptions() == OverwriteMode_T::Disallow &&
      std::filesystem::exists(Filename, ec))
   { // file we are attempting to create already exists
      ymLog(VF::Errstream, "File (or directory) '{}' already exists", Filename);
   }
   else if (ec)
   { // filesystem failure
      ymLog(VF::Errstream, "Filesystem error when attempting to open '{}' with error code {}", Filename, ec.value());
   }
   else
   { // open!
      _file = std::fopen(Filename.data(), "w"); // status of failure handled at call site
   }
}

/** openOutfile_appendTimeStamp
 *
 * @brief Attempts to open the file with the current time appended to the file name.
 * 
 * @param Filename -- Name of file.
 */
void ym::Logger::openOutfile_appendTimeStamp(std::string_view const Filename) noexcept
{
   auto extPos = Filename.find_last_of('.');
   if (extPos == 0uz ||                  // hidden files
       extPos == std::string_view::npos) // no extension
   { // no extension found
      extPos = Filename.size();
   }

   std::string_view ext{""};
   try
   { // get extension
      ext = Filename.substr(extPos);
   }
   catch (std::out_of_range const & E)
   { // logic error
      ymLog(VF::Errstream, "Trouble finding extension. {}", E.what());
   }

   constexpr std::string_view TimeStamp("_YYYY_mm_dd_HH_MM_SS");

   std::array<char, 1024uz> buffer{'\0'};
   std::pmr::monotonic_buffer_resource mbr{buffer.data(), buffer.size(), std::pmr::null_memory_resource()};
   std::pmr::string stampedFilename(Filename.size() + TimeStamp.size(), '\0', &mbr);

   auto updatedStemSize = Filename.size() - ext.size();

   // TODO this is a great time to use YMASSERT with a custom handler...

   if (stampedFilename.size() >= buffer.size())
   { // cannot fit desired filename in character limit (-1 for null terminator)
      ymLog(VF::Errstream, "Not enough room for time stamped filename '{}'", Filename);
      updatedStemSize = buffer.size() - 1uz - TimeStamp.size() - ext.size(); // -1 for null terminator
   }

   try
   {
      // write stem
      auto result = fmt::format_to_n(
         stampedFilename.data(),
         updatedStemSize,
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

      if ((result.out != &*stampedFilename.end()) ||
         (*result.out != '\0'))
      { // unexpected error writing time stamp
         ymLog(VF::Errstream, "Trouble printing time stamp. Stem {}, Stamp {}, Ext {}",
            updatedStemSize, TimeStamp.size(), ext.size());
         stampedFilename = Filename;
      }
   }
   catch (std::exception const & E)
   { // logic or formatting error
      ymLog(VF::Errstream, "fmt::format_to_n encountered an error. {}", E.what());
   }

   openOutfile_core(stampedFilename);
}
