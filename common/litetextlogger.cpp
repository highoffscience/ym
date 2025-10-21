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
   str       const   Filename,
   Options_T const & Options) :
      TextLogger(Filename),
      _Options  {Options }
{ }

/**
 * @brief TODO
 * 
 * @param Format 
 * @param args 
 */
void ym::LiteTextLogger::producer(
   str const        Format,
   fmt::format_args args)
{

}
