/**
 * @author Forrest Jablonski
 */

#include "logger.h"

/**
 *
 */
ym::Logger::Logger(void)
   : _outfile_ptr {nullptr}
{
}

/**
 *
 */
ym::Logger::~Logger(void)
{
   closeOutfile();
}

/**
 *
 */
bool ym::Logger::openOutfile(str const Filename)
{
   if (!_outfile_ptr)
   {
      _outfile_ptr = std::fopen(Filename, "w");
   }

   return _outfile_ptr != nullptr;
}

/**
 *
 */
void ym::Logger::closeOutfile(void)
{
   std::fclose(_outfile_ptr);
}
