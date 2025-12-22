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
 * @throws Whatever openOutfile_appendTimeStamp() throws.
 * 
 * @param Filename -- Name of file.
 * @param Options  -- List of optional modes.
 * 
 * @note We open the file here instead of the constructor to allow flexibility with
 *       derived classes handling the file operations. Also it's awkward to code
 *       constructors that throw.
 *
 * @returns bool -- True if the file is/was opened, false otherwise.
 */
bool ym::Logger::openOutfile(
   std::string_view const   Filename,
   Options_T        const & Options)
{
   if (!isOutfileOpened())
   { // file not opened
      if (Options == FilenameMode_T::AppendTimeStamp)
      { // append file stamp
         openOutfile_appendTimeStamp(Filename, Options);
      }
      else
      { // do not append file stamp (default fallthrough)
         openOutfile_core(Filename, Options);
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
 * @param Options  -- List of optional opening modes.
 */
void ym::Logger::openOutfile_core(
   std::string_view const   Filename,
   Options_T        const & Options) noexcept
{
   std::error_code ec;
   if (Options == OverwriteMode_T::Disallow &&
      std::filesystem::exists(Filename, ec))
   { // file we are attempting to create already exists
      ymLog(VF::Errstream, "WARNING: File (or directory) '{}' already exists", Filename);
   }
   else if (ec)
   { // filesystem failure
      ymLog(VF::Errstream, "WARNING: Filesystem error when attempting to open '{}' with error code {}", Filename, ec.value());
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
 * @throws OpenError -- If a logic error occurs.
 * @throws OpenError -- Error printing time stamp.
 * 
 * @param Filename -- Name of file.
 * @param Options  -- List of optional modes.
 */
void ym::Logger::openOutfile_appendTimeStamp(
   std::string_view const   Filename,
   Options_T        const & Options) noexcept
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
      ymLog(VF::Errstream, "WARNING: Trouble finding extension. {}", E.what());
   }

   constexpr std::string_view TimeStamp("_YYYY_mm_dd_HH_MM_SS");

   std::array<char, 256u> buffer{'\0'};
   std::pmr::monotonic_buffer_resource mbr{buffer.data(), buffer.size(), std::pmr::null_memory_resource()};
   std::pmr::string timeStampedFilename(Filename.size() + TimeStamp.size(), '\0', &mbr);

   try
   {
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

      if (result.out != &*timeStampedFilename.end())
      { // unexpected error writing time stamp
         ymLog(VF::Errstream, "WARNING: Trouble printing time stamp. {} -- {}",
            (void*)result.out, (void*)&*timeStampedFilename.end());
      }
   }
   catch (std::exception const & E)
   { // logic or formatting error
      ymLog(VF::Errstream, "WARNING: fmt::format_to_n encountered an error. {}", E.what());
   }

   openOutfile_core(timeStampedFilename, Options);
}
