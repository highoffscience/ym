/**
 * @author Forrest Jablonski
 */

#include "timer.h"

/**
 *
 */
ym::Timer::Timer(void)
   : _startTime {Clock_T::now()}
{
}

/**
 *
 */
void ym::Timer::reset(void)
{
   _startTime = Clock_T::now();
}
