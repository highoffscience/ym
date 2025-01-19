/**
 * @file    loggable.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include <type_traits>

namespace ym
{

template <typename T>
constexpr auto ym_is_loggable_v = 
   std::disjunction<
      std::is_arithmetic<T>,
      std::is_pointer<T>
   >::value;

/** Loggable
 *
 * @brief Represents a supported loggable data type.
 *
 * @ref <https://en.cppreference.com/w/cpp/io/c/fprintf>.
 *
 * @tparam T -- Type that is loggable.
 */
#if (YM_CPP_STANDARD >= 20)
   template <typename T>
   concept Loggable = ym_is_loggable_v<T>;
   #define YM_ENABLEIF
#else
   #define Loggable typename
   #define YM_ENABLEIF , std::enable_if_t<ym_is_loggable_v<T>, int> = 0
#endif

} // ym
