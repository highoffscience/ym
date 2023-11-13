/**
 * @file    signal.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "../common/ymdefs.h"

#include <type_traits>

namespace ym::hsm
{

using SignalBase_T = uint32;

static_assert(std::is_unsigned_v<std::decay_t<SignalBase_T>>, "Signal base type must decay to unsigned int");

/**
 * TODO
 */
template <typename T>
concept Signalable = std::is_same_v<std::decay_t<T>, SignalBase_T>;

} // ym::hsm
