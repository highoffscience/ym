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
char ym::Ops::castToChar(str const S)
{
   OpsError_BadCastToChar::check(S && S[0_u32] && !S[1_u32], "String '%s' not a valid char", S);
   return S[0_u32];
}

/**
 * TODO
 */
template <>
auto ym::Ops::castTo<ym::int8>(str    const S,
                               uint32 const Base) -> int8
{
   errno = 0_i32;
   auto const Val = std::strtol(S, nullptr, Base);
   OpsError_BadCastToInt8::check(errno == 0_i32, "String '%s' not a valid int8 (errno %d)", S, errno);
   return static_cast<int8>(Val);
}

/**
 * TODO
 */
template <>
auto ym::Ops::castTo<ym::int16>(str    const S,
                                uint32 const Base) -> int16
{
   errno = 0_i32;
   auto const Val = std::strtol(S, nullptr, Base);
   OpsError_BadCastToInt16::check(errno == 0_i32, "String '%s' not a valid int16 (errno %d)", S, errno);
   return static_cast<int16>(Val);
}

/**
 * TODO
 */
template <>
auto ym::Ops::castTo<ym::int32>(str    const S,
                                uint32 const Base) -> int32
{
   errno = 0_i32;
   auto const Val = std::strtol(S, nullptr, Base);
   OpsError_BadCastToInt32::check(errno == 0_i32, "String '%s' not a valid int32 (errno %d)", S, errno);
   return static_cast<int32>(Val);
}

/**
 * TODO
 */
template <>
auto ym::Ops::castTo<ym::int64>(str    const S,
                                uint32 const Base) -> int64
{
   errno = 0_i32;
   auto const Val = std::strtoll(S, nullptr, Base);
   OpsError_BadCastToInt64::check(errno == 0_i32, "String '%s' not a valid int64 (errno %d)", S, errno);
   return static_cast<int64>(Val);
}

/**
 * TODO
 */
template <>
auto ym::Ops::castTo<ym::uint8>(str    const S,
                                uint32 const Base) -> uint8
{
   errno = 0_i32;
   auto const Val = std::strtoul(S, nullptr, Base);
   OpsError_BadCastToUInt8::check(errno == 0_i32, "String '%s' not a valid uint8 (errno %d)", S, errno);
   return static_cast<uint8>(Val);
}

/**
 * TODO
 */
template <>
auto ym::Ops::castTo<ym::uint16>(str    const S,
                                 uint32 const Base) -> uint16
{
   errno = 0_i32;
   auto const Val = std::strtoul(S, nullptr, Base);
   OpsError_BadCastToUInt16::check(errno == 0_i32, "String '%s' not a valid uint16 (errno %d)", S, errno);
   return static_cast<uint16>(Val);
}

/**
 * TODO
 */
template <>
auto ym::Ops::castTo<ym::uint32>(str    const S,
                                 uint32 const Base) -> uint32
{
   errno = 0_i32;
   auto const Val = std::strtoul(S, nullptr, Base);
   OpsError_BadCastToUInt32::check(errno == 0_i32, "String '%s' not a valid uint32 (errno %d)", S, errno);
   return static_cast<uint32>(Val);
}

/**
 * TODO
 */
template <>
auto ym::Ops::castTo<ym::uint64>(str    const S,
                                 uint32 const Base) -> uint64
{
   errno = 0_i32;
   auto const Val = std::strtoull(S, nullptr, Base);
   OpsError_BadCastToUInt64::check(errno == 0_i32, "String '%s' not a valid uint64 (errno %d)", S, errno);
   return static_cast<uint64>(Val);
}

/**
 * TODO
 */
template <>
auto ym::Ops::castTo<ym::float32>(str const S) -> float32
{
   errno = 0_i32;
   auto const Val = std::strtof(S, nullptr);
   OpsError_BadCastToFlt32::check(errno == 0_i32, "String '%s' not a valid float32 (errno %d)", S, errno);
   return static_cast<float32>(Val);
}

/**
 * TODO
 */
template <>
auto ym::Ops::castTo<ym::float64>(str const S) -> float64
{
   errno = 0_i32;
   auto const Val = std::strtod(S, nullptr);
   OpsError_BadCastToFlt64::check(errno == 0_i32, "String '%s' not a valid float64 (errno %d)", S, errno);
   return static_cast<float64>(Val);
}

/**
 * TODO
 */
template <>
auto ym::Ops::castTo<ym::float80>(str const S) -> float80
{
   errno = 0_i32;
   auto const Val = std::strtold(S, nullptr);
   // TODO confirm long double is at least float80
   OpsError_BadCastToFlt32::check(errno == 0_i32, "String '%s' not a valid float80 (errno %d)", S, errno);
   return static_cast<float80>(Val);
}
