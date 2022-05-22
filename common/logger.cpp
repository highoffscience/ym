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
bool ym::Logger::isOpen(void) const
{
   return _outfile.is_open();
}

/**
 *
 */
bool ym::Logger::open(str const Filename)
{
   bool wasOpened = false;

   if (!_outfile.is_open())
   {
      _outfile.open(Filename);
      wasOpened = _outfile.is_open();
   }

   return wasOpened;
}

/**
 *
 */
void ym::Logger::close(void)
{
   _outfile.close();
}
