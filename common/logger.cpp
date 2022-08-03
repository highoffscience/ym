/**
 * @author Forrest Jablonski
 */

#include "logger.h"

/**
 *
 */
ym::Logger::Logger(void)
   : _outfile_uptr {nullptr, [](std::FILE * const Ptr) { std::fclose(Ptr); }}
{
}

/**
 *
 */
bool ym::Logger::openOutfile(str const Filename)
{
   if (!_outfile_uptr)
   {
      _outfile_uptr.reset(std::fopen(Filename, "w"));
   }

   return static_cast<bool>(_outfile_uptr);
}
