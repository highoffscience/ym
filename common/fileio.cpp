/**
 * @file    fileio.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "fileio.h"

#include "globallogger.h"

#include <sys/stat.h>

#include <filesystem>
#include <tuple>

/** exists
 *
 * @brief Resets the file handle.
 *
 * TODO
 */
bool ym::FileIO::exists(str const Filename) noexcept
{
   auto found = false;

   struct stat st;
   if (stat(Filename, &st) == 0)
   { // found file
      found = true;
   }
#if (YM_USE_HEAP_AS_FALLBACK)
   else
   {
      found = std::filesystem::is_regular_file(Filename.get()); // uses heap
   }
#endif

   return found;
}

/** reset
 *
 * @brief Resets the file handle.
 *
 * TODO
 */
bool ym::FileIO::reset(
   str const Filename,
   str const Mode = "rb") noexcept
{
   if (isOpen())
   { // close current file
      std::ignore = std::fclose(get());
   }

   _file = std::fopen(Filename, Mode);

   struct stat st;
   if (stat(Filename, &st) == 0)
   { // got file size
      _size = static_cast<std::size_t>(st.st_size);
   }
#if (YM_USE_HEAP_AS_FALLBACK)
   else
   { // failed to query file size - use alternative method
      std::error_code ec;
      _size = std::filesystem::file_size(Filename.get(), ec); // uses heap

      if (ec)
      { // failed to query file size - again
         ymLog(VF::Warning, "Couldn't get size for file {}", Filename);
         _size = 0uz;
      }
   }
#endif

   if (getSize() == 0uz)
   { // failed to acquire size
      std::ignore = std::fclose(get());
      _file = nullptr;
   }

   return isOpen();
}

/** createFileBuffer
 *
 * @brief Reads in file contents into an std::string.
 *
 * @returns std::optional<std::string> -- File contents, or null if an error occured.
 */
#if (YM_USE_HEAP_AS_FALLBACK)
std::optional<std::string> ym::FileIO::createFileBuffer(void) noexcept
{
   std::optional<std::string> buffer; // default is nullopt

   if (isOpen())
   { // file opened
      std::string contents;
      contents.resize_and_overwrite(getSize(), [this](char * const buf_Ptr, std::size_t const N) {
         return std::fread(buf_Ptr, 1uz, N, this->get());
      });
      buffer = std::move(contents);
   }

   return buffer;
}
#endif

/** createFileBuffer
 *
 * @brief Reads in file contents into a supplied buffer.
 *
 * @param Filename -- Name of file to read from.
 *
 * @returns bool -- True if file was read and copied successfully, false if an error occured.
 */
bool ym::FileIO::createFileBuffer(std::span<char> buffer) noexcept
{
   auto success = false;

   if (isOpen())
   { // file opened
      if (buffer.size() > getSize())
      { //
         auto const NRead = std::fread(buffer.data(), 1uz, getSize(), get());
         // TODO do we rewind? even if successful.
         // TODO rename to fillBuffer
         // TODO create a re-entrant version so we can load a large file with many chunks.
         success = (NRead == getSize());
      }
   }

   return success;
}
