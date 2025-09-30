/**
 * @file    fileio.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "fileio.h"

#include "textlogger.h"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <system_error>
#include <utility>

/** createFileBuffer
 * 
 * @brief Reads in file contents into an std::pmr::string.
 * 
 * @param Filename -- Name of file to read from.
 * 
 * @returns std::optional<std::pmr::string> -- File contents, or null if an error occured.
 */
std::optional<std::pmr::string> ym::FileIO::createFileBuffer(str const Filename)
{
   std::optional<std::pmr::string> buffer; // default is nullopt

   if (std::ifstream infile(Filename.get()); infile.is_open())
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

         std::pmr::string contents;
         contents.resize_and_overwrite(Size_bytes, [&infile](char * const buf_Ptr, sizet const N) {
            (void)infile.read(buf_Ptr, N);
            return N;
         });

         if (infile.good())
         { // file read into memory successful
            buffer = std::move(contents);
         }
         else
         { // error reading file
            str const Flag =
               infile.eof () ? "EOF"_str  :
               infile.fail() ? "FAIL"_str :
               infile.bad () ? "BAD"_str  : "?"_str;
            ymLog(VG::Warning, "Got error {} while attempting to read from {}", Flag, Filename);
         }
      }
   }

   return buffer;
}
