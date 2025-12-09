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
      TextLogger(Filename),
      _Options {Options}
{ }

/** isOpen
 * 
 * @brief Returns whether the logger is ready to print to or not.
 * 
 * @returns True if logger can be printed to, false otherwise.
 */
bool ym::LiteLogger::isOpen(void) const
{
   return isOutfileOpened();
}

/** open
 * 
 * @brief Prepares the logger to be written to.
 * 
 * @returns True if logger can be printed to, false otherwise.
 */
bool ym::LiteLogger::open(void)
{
   return openOutfile(getFilename().get(), getOptions());
}

/** close
 * 
 * @brief Closes the logger.
 */
void ym::LiteLogger::close(void)
{
   closeOutfile();
}

/** producer
 * 
 * @brief Writes the Format string to file.
 * 
 * @param Format -- Format string.
 * @param args   -- Arguments.
 */
void ym::LiteLogger::producer(
   strlit const     Format,
   fmt::format_args args)
{
   
}
