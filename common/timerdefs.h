/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <chrono>

namespace ym
{

typedef std::chrono::steady_clock Clock_T;
typedef Clock_T::time_point       Time_T;
typedef int64                     TimeRep_T;

/*
 * Convenience functions.
 * -------------------------------------------------------------------------- */

template <typename TimeUnit_T /*eg std::milli*/>
inline TimeRep_T ymGetGlobalTime(void);

template <typename TimeUnit_T /*eg std::milli*/>
inline TimeRep_T ymGetElapsedTime(Time_T const StartTime);

/* -------------------------------------------------------------------------- */

/**
 *
 */
class Timer
{
public:
   YM_NO_DEFAULT(Timer)

   template <typename TimeUnit_T>
   friend TimeRep_T ymGetGlobalTime(void);

private:
   static Time_T const _s_GlobalStartTime;
};

/**
 * TimeUnit_T is std::milli, for example
 */
template <typename TimeUnit_T>
inline TimeRep_T ymGetElapsedTime(Time_T const StartTime)
{
   return std::chrono::duration_cast<std::chrono::duration<TimeRep_T, TimeUnit_T>>(Clock_T::now() - StartTime).count();
}

/**
 * TimeUnit_T is std::milli, for example
 */
template <typename TimeUnit_T>
inline TimeRep_T ymGetGlobalTime(void)
{
   return ymGetElapsedTime<TimeUnit_T>(Timer::_s_GlobalStartTime);
}

} // ym
