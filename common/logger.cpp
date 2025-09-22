/**
 * @file    logger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "logger.h"

#include "textlogger.h"
#include "timer.h"

#include "fmt/chrono.h"
#include "fmt/format.h"

#include <array>
#include <exception>
#include <filesystem>
#include <memory_resource>
#include <string>
#include <system_error>

/** Logger
 *
 * @brief Constructor.
 *
 * @note _outfile_uptr is set to null to serve as a flag that the logger is uninitialized.
 */
ym::Logger::Logger(void) :
   _outfile_uptr {nullptr,
      [] (std::FILE * const file_Ptr) {
         if (file_Ptr != stdout &&
             file_Ptr != stderr) { // don't try to close standard streams
            [[maybe_unused]]
            auto const RetVal = std::fclose(file_Ptr);
         }
      }
   }
{ }

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
   OpeningOptions_T const & Options)
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

/** openOutfile_core
 *
 * @brief Attempts to open a write-file.
 * 
 * @param Filename -- Name of file.
 * @param Options  -- List of optional opening modes.
 */
void ym::Logger::openOutfile_core(
   std::string_view const   Filename,
   OpeningOptions_T const & Options)
{
   std::error_code ec;
   if (Options == OverwriteMode_T::Disallow &&
      std::filesystem::exists(Filename, ec))
   { // file we are attempting to create already exists
      ymLog(VG::Warning, "WARNING: File (or directory) '{}' already exists", Filename);
   }
   else if (ec)
   { // filesystem failure
      ymLog(VG::Warning, "WARNING: Filesystem error when attempting to open '{}' with error code {}", Filename, ec.value());
   }
   else
   { // open!
      _outfile_uptr.reset(std::fopen(Filename.data(), "w")); // status of failure handled at call site
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
   OpeningOptions_T const & Options)
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

   YMASSERT(result.out == &*timeStampedFilename.end(), OpenError, YM_DAH,
      "Error printing time stamp. {} -- {}", (void*)result.out, (void*)&*timeStampedFilename.end())

   openOutfile_core(timeStampedFilename, Options);
}

/** closeOutfile
 *
 * @brief Closes the file and disassociates the file handle.
 */
void ym::Logger::closeOutfile(void)
{
   _outfile_uptr.reset(nullptr);
}
