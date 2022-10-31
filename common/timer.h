/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <chrono>
#include <type_traits>

namespace ym
{

// TODO this doesn't seem right
template <typename T, int a, int b>
concept LL_T = std::is_same_v<T, std::ratio<a, b>>;

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

   template <typename TimeUnit_T /* eg std::nano */>
   TimeRep_T getStartTime(void) const;

   // TODO
   template <typename TimeUnit_T /* eg std::nano */>
   requires(std::chrono::__is_ratio<TimeUnit_T>::value)
   TimeRep_T getElapsedTime(void) const;

private:
   Time_T _startTime;
};

/**
 *
 */
template <typename TimeUnit_T /* eg std::nano */>
requires(std::chrono::__is_ratio<TimeUnit_T>::value)
auto Timer::getElapsedTime(void) const -> TimeRep_T
{
   return std::chrono::duration_cast<std::chrono::duration<TimeRep_T, TimeUnit_T>>(Clock_T::now() - _startTime).count();
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
