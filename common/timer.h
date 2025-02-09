/**
 * @file    timer.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include <chrono>

namespace ym
{

/** Timer
 *
 * @brief Provides basic timing functionality.
 */
class Timer
{
public:
   using Clock_T    = std::chrono::high_resolution_clock;
   using Time_T     = Clock_T::time_point;
   using Duration_T = std::chrono::duration<int64, std::nano>;

   Timer(void);

   void reset(void);

   inline Duration_T getElapsedTime(void) const;

private:
   Time_T _startTime;
};

/** getElapsedTime
 *
 * @brief Returns the elapsed time.
 *
 * @returns Duration_T -- Duration of elapsed time.
 */
auto Timer::getElapsedTime(void) const -> Duration_T
{
   return Clock_T::now() - _startTime;
}

} // ym
