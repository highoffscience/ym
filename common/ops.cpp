/**
 * @file    ops.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ops.h"

#include <cstdlib>

/**
 * TODO
 */
template <>
auto ym::Ops::castTo<int8>(str    const S,
                           uint32 const Base) -> int8
{
   errno = 0_i32;
   auto const Val = std::strtol(S, nullptr, Base);
   OpsError_BadCastToInt8::check(errno == 0_i32, "String '%s' not a valid int8", S);
   return static_cast<int8>(Val);
}
