/**
 * @file    fileio.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "fileio.h"

#include "memio.h"
#include "textlogger.h"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <system_error>

/** createFileBuffer
 * 
 * @brief Reads in file contents as an std::string.
 * 
 * @param Filename -- Name of file to read from.
 * 
 * @returns std::string -- File contents.
 */
std::string ym::FileIO::createFileBuffer(str const Filename)
{
   std::string buffer;

   if (std::ifstream infile(Filename); infile.is_open())
   { // file opened
      std::error_code ec;
      auto const Size_bytes = std::filesystem::file_size(Filename, ec);

      if (ec)
      { // failed to get size - read file in like a peasant
         std::istreambuf_iterator<char> it(infile);
         std::istreambuf_iterator<char> end;
         buffer = {it, end};
      }
      else
      { // read in everything all at once
         buffer.resize(Size_bytes);
         infile.read(buffer.data(), Size_bytes);
      }
   }
   else
   { // file failed to open
      FileIOError::check(false, "File %s failed to open", Filename);
   }

   return buffer;
}
