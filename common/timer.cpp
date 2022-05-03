/**
 * @author Forrest Jablonski
 */

#include "timer.h"

/**
 *
 */
ym::Timer::Timer(void)
   : _startTime {clock_t::now()}
{
}

/**
 *
 */
void ym::Timer::reset(void)
{
   _startTime = clock_t::now();
}
