/**
 * @file    fileio.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "fileio.h"

#include "textlogger.h" // TODO

#include <filesystem>
#include <fstream>
#include <iterator>
#include <system_error>

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
   std::optional<std::pmr::string> buffer(std::nullopt); // until explicitly constructed

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
         buffer.emplace(Size_bytes, '\0');
         infile.read(buffer->data(), Size_bytes);
      }
   }

   return buffer;
}
