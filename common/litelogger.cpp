/**
 * @file    litelogger.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "litelogger.h"

#include "fmt/format.h"

#include <utility>

/** LiteLogger
 *
 * @brief Constructor.
 * 
 * @param Filename -- Name of file to open.
 * @param Options  -- List of optional modes.
 */
ym::LiteLogger::LiteLogger(
   strlit    const   Filename,
   Options_T const & Options) :
      TextLogger(Filename)
{ }

/**
 * @brief TODO
 */
auto ym::LiteLogger::getOptions(void) const -> Options_T const &
{
   return _Options;
}

/**
 * @brief TODO
 */
void ym::LiteLogger::close(void)
{
   std::ignore = std::fclose(_file.unwrap());
}

/**
 * @brief TODO
 * 
 * @param Format 
 * @param args 
 */
void ym::LiteLogger::producer(
   strlit const     Format,
   fmt::format_args args)
{
   
}
