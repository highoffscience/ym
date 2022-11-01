/**
 * @file    timer.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

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
   using Clock_T   = std::chrono::steady_clock;
   using Time_T    = Clock_T::time_point;
   using TimeRep_T = int64;

   Timer(void);

   void reset(void);

   template <typename TimeUnit_T> // eg std::nano
   TimeRep_T getStartTime(void) const;

   template <typename TimeUnit_T> // eg std::nano
   TimeRep_T getElapsedTime(void) const;

private:
   Time_T _startTime;
};

/** getElapsedTime
 *
 * @brief Returns the elapsed time.
 *
 * @note TimeUnit_T is std::nano, for example.
 *
 * @tparam TimeUnit_T -- Unit of returned time will be represented in.
 *
 * @return TimeRep_T -- Elapsed time in specified units.
 */
template <typename TimeUnit_T>
auto Timer::getElapsedTime(void) const -> TimeRep_T
{
   return
      std::chrono::duration_cast<
         std::chrono::duration<
            TimeRep_T, TimeUnit_T
         >
      >(Clock_T::now() - _startTime).count();
}

 /** getStartTime
 *
 * @brief Returns the start time.
 *
 * @note TimeUnit_T is std::nano, for example.
 *
 * @tparam TimeUnit_T -- Unit of returned time will be represented in.
 *
 * @return TimeRep_T -- Start time in specified units.
 */
template <typename TimeUnit_T>
auto Timer::getStartTime(void) const -> TimeRep_T
{
   return
      std::chrono::duration_cast<
         std::chrono::duration<
            TimeRep_T, TimeUnit_T
         >
      >(_startTime).count();
}

} // ym
