/**
 * @file    eventid.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "../common/ymdefs.h"

#include <type_traits>

namespace ym::hsm
{

using EventId_T = uint64;

static_assert(std::is_integral_v<EventId_T>, "Must decay to integral type");
static_assert(std::is_same_v<EventId_T, uintptr>, "Unexpected type");
static_assert(std::is_same_v<EventId_T, uint64>,  "Unexpected type");

} // ym::hsm
