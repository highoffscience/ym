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
bool ym::LiteLogger::isOpen(void) const
{
   return isOutfileOpened();
}

/**
 * @brief TODO
 */
bool ym::LiteLogger::open(void)
{
   return openOutfile(getFilename().get(), getOptions());
}

/**
 * @brief TODO
 */
void ym::LiteLogger::close(void)
{
   closeOutfile();
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
