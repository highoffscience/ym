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

/**
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

/**
 * @brief Resets the file handle.
 *
 * TODO
 */
bool ym::FileIO::reset(
   str const Filename,
   str const Mode) noexcept
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

/**
 * @brief Reads in file contents into an std::string.
 *
 * @returns std::optional<std::string> -- File contents, or null if an error occured.
 */
#if (YM_USE_HEAP_AS_FALLBACK)
   std::optional<std::string> ym::FileIO::createAndFillBuffer(void) noexcept
   {
      std::optional<std::string> buffer; // default is nullopt

      if (isOpen())
      { // file opened
         std::string contents;
         contents.resize_and_overwrite(getSize(), [this](char * const buf_Ptr, std::size_t const N) {
            return std::fread(buf_Ptr, 1uz, N, this->get());
         });
         buffer = std::move(contents);
         std::rewind(get());
      }

      return buffer;
   }
#endif

/**
 * @brief Reads entire file contents into a supplied buffer.
 *
 * @note The size of the file is already calculated and query-able. If and only if
 *       this function returns true has all the data been read.
 *
 * @param buffer     -- Buffer to read file into.
 * @param AppendNull -- Whether or not to apply null terminator.
 *
 * @returns bool -- True if file was read and copied successfully, false if an error occured.
 */
bool ym::FileIO::fillBuffer(
   std::span<char> buffer,
   bool const      AppendNull) noexcept
{
   auto success = false;

   if (isOpen())
   { // file opened
      if (buffer.size() >= getSize())
      { // tentatively enough room to store data
         auto const NRead = std::fread(buffer.data(), 1uz, buffer.size(), get());
         success = (NRead == getSize());

         if (AppendNull)
         { // apply null terminator
            if (buffer.size() > getSize())
            { // we have room
               buffer[NRead] = '\0';
            }
            else
            { // not enough room afterall
               success = false;
            }
         }

         std::rewind(get());
      }
   }

   return success;
}

/**
 * @brief Reads in file contents incrementally into a supplied buffer.
 *
 * @note Since the user has the file size available to them, it is the user's responsibility
 *       to track how much file data has bbeen read thus far. And return value of true means
 *       that data has been read, a return value of false means that either EOF or an error
 *       has occured.
 *
 * @param buffer -- Buffer to read file into.
 *
 * @returns std::optional<std::span<char>> -- If true, all available or requested amount of data was read.
 *                                            If false, EOF or an error occured.
 */
std::optional<std::span<char>> ym::FileIO::fillBufferPiecewise(std::span<char> buffer) noexcept
{
   std::optional<std::span<char>> data; // default is nullopt

   if (isOpen())
   { // file opened
      auto const NRead = std::fread(buffer.data(), 1uz, buffer.size(), get());
      if (NRead > 0uz)
      { // at least some data was available
         data = buffer.subspan(0uz, NRead);
      }
   }

   return data;
}
