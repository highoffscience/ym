/**
 * @file    litetextlogger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "litetextlogger.h"

#include "fmt/format.h"

/** LiteTextLogger
 *
 * @brief Constructor.
 * 
 * @param Filename -- Name of file to open.
 * @param Options  -- List of optional modes.
 */
ym::LiteTextLogger::LiteTextLogger(
   strlit    const   Filename,
   Options_T const & Options) :
      TextLogger(Filename)
{ }

/**
 * @brief TODO
 */
void ym::LiteTextLogger::closeOutfile(void)
{
   std::ignore = std::fclose(_file.unwrap());
   _file = nullptr;
}

/**
 * @brief TODO
 * 
 * @param Format 
 * @param args 
 */
void ym::LiteTextLogger::producer(
   strlit const     Format,
   fmt::format_args args)
{
   
}
