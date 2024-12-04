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
