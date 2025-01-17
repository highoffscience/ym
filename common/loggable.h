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

/*
#define USE_CPP20 1

template <typename T>
constexpr auto ym_is_loggable_v = std::disjunction<std::is_integral<T>>::value;

#if USE_CPP20

template <typename T>
concept Loggable = ym_is_loggable_v<T>;

#define ENABLEIF

#else

#define Loggable typename
#define ENABLEIF , std::enable_if_t<ym_is_loggable_v<T>, int> = 0

#endif

template <Loggable T ENABLEIF>
void print(T const t)
{
   std::cout << t << std::endl;
}
*/

/** Loggable
 *
 * @brief Represents a supported loggable data type.
 *
 * @ref <https://en.cppreference.com/w/cpp/io/c/fprintf>.
 *
 * @tparam T -- Type that is loggable.
 */
template <typename T>
#if (YM_CPP_STANDARD >= 20)
   concept Loggable
#else
   constexpr auto Loggable
#endif
   = std::is_fundamental_v<T> || std::is_pointer_v<T>;

} // ym
