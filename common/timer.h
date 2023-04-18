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

/** Timeable_T
 * 
 * @brief Represents a unit of time (std::ratio).
 * 
 * @tparam T -- Data type.
 */
template <typename T>
concept Timeable = requires(T)
{
   T::num;
   T::den;
};

/** Timer
 *
 * @brief Provides basic timing functionality.
 */
class Timer
{
public:
   using Clock_T = std::chrono::steady_clock;
   using Time_T  = Clock_T::time_point;
   using Rep_T   = int64;

   Timer(void);

   void reset(void);

   template <Timeable Timeable_T = std::nano>
   Rep_T getElapsedTime(void) const;

private:
   Time_T _startTime;
};

/** getElapsedTime
 *
 * @brief Returns the elapsed time.
 *
 * @tparam Timeable_T -- Unit of returned time will be represented in.
 *
 * @returns Rep_T -- Elapsed time in specified units.
 */
template <Timeable Timeable_T>
auto Timer::getElapsedTime(void) const -> Rep_T
{
   return
      std::chrono::duration_cast<
         std::chrono::duration<
            Rep_T, Timeable_T
         >
      >(Clock_T::now() - _startTime).count();
}

} // ym
