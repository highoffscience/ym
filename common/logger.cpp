/**
 * @author Forrest Jablonski
 */

#include "logger.h"

/**
 *
 */
ym::Logger::Logger(void)
   : _outfile {}
{
}

/**
 *
 */
bool ym::Logger::open(str const Filename)
{
   bool didOpen = false; // until told otherwise

   if (!_outfile.is_open())
   {
      _outfile.open(Filename);
      didOpen = _outfile.is_open();
   }

   return didOpen;
}

/**
 *
 */
void ym::Logger::close(void)
{
   _outfile.close();
}
