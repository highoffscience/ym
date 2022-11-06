/**
 * @file    loggable.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <type_traits>

namespace ym
{

/** Loggable_T
 *
 * @brief Represents a supported loggable data type.
 *
 * @ref <https://en.cppreference.com/w/cpp/io/c/fprintf>
 *
 * @tparam T -- Type that is loggable
 */
template <typename T>
concept Loggable_T = std::is_fundamental_v<T> ||
                     std::is_pointer_v    <T> ;

} // ym
