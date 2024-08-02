/**
 * @file    fileio.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "ymerror.h"

#include <string>

namespace ym
{

/** FileIO
 * 
 * @brief Provides file IO and manipulation functions.
 */
class FileIO
{
public:
   YM_NO_DEFAULT(FileIO)

   YM_DECL_YMERROR(FileIOError)

   static std::string createFileBuffer(str const Filename);
};

} // ym
