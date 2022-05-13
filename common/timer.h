/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <chrono>

namespace ym
{

/**
 *
 */
class Timer
{
public:
   typedef std::chrono::steady_clock Clock_T;
   typedef Clock_T::time_point       Time_T;
   typedef int64                     TimeRep_T;

   Timer(void);

   void reset(void);

   template <typename TimeUnit_T /* eg std::nano */>
   TimeRep_T getStartTime(void) const;

   template <typename TimeUnit_T /* eg std::nano */>
   TimeRep_T getElapsedTime(void) const;

private:
   Time_T _startTime;
};

/**
 *
 */
template <typename TimeUnit_T /* eg std::nano */>
auto Timer::getElapsedTime(void) const -> TimeRep_T
{
   return std::chrono::duration_cast<std::chrono::duration<TimeRep_T, TimeUnit_T>>(clock_t::now() - _startTime).count();
}

/**
 *
 */
template <typename TimeUnit_T /* eg std::nano */>
auto Timer::getStartTime(void) const -> TimeRep_T
{
   return std::chrono::duration_cast<std::chrono::duration<TimeRep_T, TimeUnit_T>>(_startTime).count();
}

} // ym
