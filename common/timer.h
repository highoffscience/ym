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
   typedef std::chrono::steady_clock clock_t;
   typedef clock_t::time_point       time_t;
   typedef int64                     timerep_t;

   Timer(void);

   void reset(void);

   template <typename TimeUnit_T /* eg std::nano */>
   timerep_t getStartTime(void) const;

   template <typename TimeUnit_T /* eg std::nano */>
   timerep_t getElapsedTime(void) const;

private:
   time_t _startTime;
};

/**
 *
 */
template <typename TimeUnit_T /* eg std::nano */>
auto Timer::getElapsedTime(void) const -> timerep_t
{
   return std::chrono::duration_cast<std::chrono::duration<timerep_t, TimeUnit_T>>(clock_t::now() - _startTime).count();
}

/**
 *
 */
template <typename TimeUnit_T /* eg std::nano */>
auto Timer::getStartTime(void) const -> timerep_t
{
   return std::chrono::duration_cast<std::chrono::duration<timerep_t, TimeUnit_T>>(_startTime).count();
}

} // ym
