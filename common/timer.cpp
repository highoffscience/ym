/**
 * @file    timer.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "timer.h"

/** Timer
 *
 * @brief Constructor.
 */
ym::Timer::Timer(void) :
   _startTime {Clock_T::now()}
{}

/** reset
 *
 * @brief Resets the start time.
 */
void ym::Timer::reset(void)
{
   _startTime = Clock_T::now();
}
