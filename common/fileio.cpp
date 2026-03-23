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

/** reset
 *
 * @brief Resets the file handle.
 */
bool ym::FileIO::reset(
   str const Filename,
   str const Mode = "rb") noexcept
{
   if (isOpen())
   { // close current file
      std::fclose(get());
   }

   _file = std::fopen(Filename.get(), Mode.get());

   // TODO can Filename decay naturally? test it!
   struct stat st;
   if (stat(Filename.get(), &st) == 0)
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

   return isOpen();
}

/** createFileBuffer
 *
 * @brief Reads in file contents into an std::string.
 *
 * @returns std::optional<std::string> -- File contents, or null if an error occured.
 */
std::optional<std::string> ym::FileIO::createFileBuffer(void) noexcept
{
   std::optional<std::string> buffer; // default is nullopt

   if (isOpen())
   { // file opened
      if (getSize() > 0uz)
      { // read in everything all at once
         std::string contents;
         contents.resize_and_overwrite(getSize(), [this](char * const buf_Ptr, std::size_t const N) {
            std::ignore = std::fread(buf_Ptr, 1uz, this->getSize(), this->get());
            return N;
         });
      }
      else
      { // failed to get size - read file in like a peasant
         // TODO
         for (char buf[8]; std::fgets(buf, sizeof buf, tmpf) != nullptr;);
      }


      if (ec)
      { // failed to get size - read file in like a peasant
         std::istreambuf_iterator<char> it(infile);
         std::istreambuf_iterator<char> end;
         buffer = {it, end};
      }
      else
      { // read in everything all at once

         std::string contents;
         contents.resize_and_overwrite(Size_bytes, [&infile](char * const buf_Ptr, std::size_t const N) {
            std::ignore = infile.read(buf_Ptr, N);
            return N;
         });

         if (infile.good())
         { // file read into memory successful
            buffer = std::move(contents);
         }
         else
         { // error reading file
            str const Flag =
               infile.eof () ? "EOF"  :
               infile.fail() ? "FAIL" :
               infile.bad () ? "BAD"  : "?";
            ymLog(VF::Warning, "Got error {} while attempting to read from {}", Flag, Filename);
         }
      }
   }

   return buffer;
}

/** createFileBuffer
 *
 * @brief Reads in file contents into a supplied buffer.
 *
 * @param Filename -- Name of file to read from.
 *
 * @returns bool -- True if file was read and copied successfully, false if an error occured.
 */
bool ym::FileIO::createFileBuffer(
   str const       Filename,
   std::span<char> buffer) noexcept
{
   if (std::ifstream infile(Filename.get(), std::ios::binary); infile.is_open())
   { // file opened

      std::error_code ec;
      auto const Size_bytes = std::filesystem::file_size(Filename.get(), ec);

      if (ec)
      { // failed to get size - read file in like a peasant
         std::istreambuf_iterator<char> it(infile);
         std::istreambuf_iterator<char> end;
         buffer = {it, end};
      }
      else
      { // read in everything all at once

         std::string contents;
         contents.resize_and_overwrite(Size_bytes, [&infile](char * const buf_Ptr, std::size_t const N) {
            std::ignore = infile.read(buf_Ptr, N);
            return N;
         });

         if (infile.good())
         { // file read into memory successful
            buffer = std::move(contents);
         }
         else
         { // error reading file
            str const Flag =
               infile.eof () ? "EOF"  :
               infile.fail() ? "FAIL" :
               infile.bad () ? "BAD"  : "?";
            ymLog(VF::Warning, "Got error {} while attempting to read from {}", Flag, Filename);
         }
      }
   }
}
