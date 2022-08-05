/**
 * @author Forrest Jablonski
 */

#include "lightlogger.h"

/**
 *
 */
ym::LightLogger::~LightLogger(void)
{
   _outfile_uptr.reset();
}
