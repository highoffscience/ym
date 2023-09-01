/**
 * @file    ops.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ops.h"

#include <limits>
#include <stdexcept>

/** castToChar
 *
 * @brief Casts string to char.
 * 
 * @throws OpsError_BadCastToChar -- If string is not a valid char.
 * 
 * @param S -- String to cast.
 * 
 * @returns char -- String as char.
 */
char ym::Ops::castToChar(str const S)
{
   OpsError_BadCast::check(S && S[0_u32] && !S[1_u32], "String '%s' not a valid char", S);
   return S[0_u32];
}

/** castTo
 *
 * @brief Casts string to int8.
 * 
 * @note std::strtoi doesn't flag an error if conversion is invalid.
 * 
 * @note Most strings will likely be small enough for small string
 *       optimization to take effect.
 * 
 * @throws OpsError_BadCast -- If string is not a valid or out of range int32.
 * @throws OpsError_BadCast -- If string is not in range of int8.
 * 
 * @param S    -- String to cast.
 * @param Base -- Radix of int8.
 * 
 * @returns int8 -- String as int8.
 */
template <>
auto ym::Ops::castTo<ym::int8>(std::string const & S,
                               uint32      const   Base) -> int8
{
   auto val_int32 = 0_i32;

   try
   {
      auto const Val = std::stoi(S, nullptr, Base);
      static_assert(std::is_same_v<decltype(Val), int32 const>, "Unexpected type");
      val_int32 = Val;
   }
   catch (std::exception const & E)
   {
      OpsError_BadCast::check(false, "String '%s' invalid or out of range int32 (%s)", S.c_str(), E.what());
   }

   OpsError_BadCast::check(
      val_int32 >= static_cast<int32>(std::numeric_limits<int8>::min()) &&
      val_int32 <= static_cast<int32>(std::numeric_limits<int8>::max()),
      "String '%s' out of int8 range", S.c_str());

   return static_cast<int8>(val_int32);
}

/** castTo
 *
 * @brief Casts string to int16.
 * 
 * @note std::strtoi doesn't flag an error if conversion is invalid.
 * 
 * @note Most strings will likely be small enough for small string
 *       optimization to take effect.
 * 
 * @throws OpsError_BadCast -- If string is not a valid or out of range int32.
 * @throws OpsError_BadCast -- If string is not in range of int16.
 * 
 * @param S    -- String to cast.
 * @param Base -- Radix of int16.
 * 
 * @returns int16 -- String as int16.
 */
template <>
auto ym::Ops::castTo<ym::int16>(std::string const & S,
                                uint32      const   Base) -> int16
{
   auto val_int32 = 0_i32;

   try
   {
      auto const Val = std::stoi(S, nullptr, Base);
      static_assert(std::is_same_v<decltype(Val), int32 const>, "Unexpected type");
      val_int32 = Val;
   }
   catch (std::exception const & E)
   {
      OpsError_BadCast::check(false, "String '%s' invalid or out of range int32 (%s)", S.c_str(), E.what());
   }

   OpsError_BadCast::check(
      val_int32 >= static_cast<int32>(std::numeric_limits<int16>::min()) &&
      val_int32 <= static_cast<int32>(std::numeric_limits<int16>::max()),
      "String '%s' out of int16 range", S.c_str());

   return static_cast<int16>(val_int32);
}

/** castTo
 *
 * @brief Casts string to int32.
 * 
 * @note std::strtoi doesn't flag an error if conversion is invalid.
 * 
 * @note Most strings will likely be small enough for small string
 *       optimization to take effect.
 * 
 * @throws OpsError_BadCast -- If string is not a valid or out of range int32.
 * 
 * @param S    -- String to cast.
 * @param Base -- Radix of int32.
 * 
 * @returns int32 -- String as int32.
 */
template <>
auto ym::Ops::castTo<ym::int32>(std::string const & S,
                                uint32      const   Base) -> int32
{
   auto val_int32 = 0_i32;

   try
   {
      auto const Val = std::stoi(S, nullptr, Base);
      static_assert(std::is_same_v<decltype(Val), int32 const>, "Unexpected type");
      val_int32 = Val;
   }
   catch (std::exception const & E)
   {
      OpsError_BadCast::check(false, "String '%s' invalid or out of range int32 (%s)", S.c_str(), E.what());
   }

   return val_int32;
}

/** castTo
 *
 * @brief Casts string to int64.
 * 
 * @note std::strtol doesn't flag an error if conversion is invalid.
 * 
 * @note Most strings will likely be small enough for small string
 *       optimization to take effect.
 * 
 * @throws OpsError_BadCast -- If string is not a valid or out of range int64.
 * 
 * @param S    -- String to cast.
 * @param Base -- Radix of int64.
 * 
 * @returns int64 -- String as int64.
 */
template <>
auto ym::Ops::castTo<ym::int64>(std::string const & S,
                                uint32      const   Base) -> int64
{
   auto val_int64 = 0_i64;

   try
   {
      auto const Val = std::stol(S, nullptr, Base);
      static_assert(std::is_same_v<decltype(Val), int64 const>, "Unexpected type");
      val_int64 = Val;
   }
   catch (std::exception const & E)
   {
      OpsError_BadCast::check(false, "String '%s' invalid or out of range int64 (%s)", S.c_str(), E.what());
   }

   return val_int64;
}

/** castTo
 *
 * @brief Casts string to uint8.
 * 
 * @note std::strtoul doesn't flag an error if conversion is invalid.
 * 
 * @note Most strings will likely be small enough for small string
 *       optimization to take effect.
 * 
 * @throws OpsError_BadCast -- If string is not a valid or out of range uint64.
 * @throws OpsError_BadCast -- If string is not in range of uint8.
 * 
 * @param S    -- String to cast.
 * @param Base -- Radix of uint8.
 * 
 * @returns uint8 -- String as uint8.
 */
template <>
auto ym::Ops::castTo<ym::uint8>(std::string const & S,
                                uint32      const   Base) -> uint8
{
   auto val_uint64 = 0_u64;

   try
   {
      auto const Val = std::stoul(S, nullptr, Base);
      static_assert(std::is_same_v<decltype(Val), uint64 const>, "Unexpected type");
      val_uint64 = Val;
   }
   catch (std::exception const & E)
   {
      OpsError_BadCast::check(false, "String '%s' invalid or out of range uint64 (%s)", S.c_str(), E.what());
   }

   OpsError_BadCast::check(
      val_uint64 >= static_cast<uint64>(std::numeric_limits<uint8>::min()) &&
      val_uint64 <= static_cast<uint64>(std::numeric_limits<uint8>::max()),
      "String '%s' out of uint8 range", S.c_str());

   return static_cast<uint8>(val_uint64);
}

/** castTo
 *
 * @brief Casts string to uint16.
 * 
 * @note std::strtoul doesn't flag an error if conversion is invalid.
 * 
 * @note Most strings will likely be small enough for small string
 *       optimization to take effect.
 * 
 * @throws OpsError_BadCast -- If string is not a valid or out of range uint64.
 * @throws OpsError_BadCast -- If string is not in range of uint16.
 * 
 * @param S    -- String to cast.
 * @param Base -- Radix of uint16.
 * 
 * @returns uint16 -- String as uint16.
 */
template <>
auto ym::Ops::castTo<ym::uint16>(std::string const & S,
                                 uint32      const   Base) -> uint16
{
   auto val_uint64 = 0_u64;

   try
   {
      auto const Val = std::stoul(S, nullptr, Base);
      static_assert(std::is_same_v<decltype(Val), uint64 const>, "Unexpected type");
      val_uint64 = Val;
   }
   catch (std::exception const & E)
   {
      OpsError_BadCast::check(false, "String '%s' invalid or out of range uint64 (%s)", S.c_str(), E.what());
   }

   OpsError_BadCast::check(
      val_uint64 >= static_cast<uint64>(std::numeric_limits<uint16>::min()) &&
      val_uint64 <= static_cast<uint64>(std::numeric_limits<uint16>::max()),
      "String '%s' out of uint16 range", S.c_str());

   return static_cast<uint16>(val_uint64);
}

/** castTo
 *
 * @brief Casts string to uint32.
 * 
 * @note std::strtoul doesn't flag an error if conversion is invalid.
 * 
 * @note Most strings will likely be small enough for small string
 *       optimization to take effect.
 * 
 * @throws OpsError_BadCast -- If string is not a valid or out of range uint64.
 * @throws OpsError_BadCast -- If string is not in range of uint32.
 * 
 * @param S    -- String to cast.
 * @param Base -- Radix of uint32.
 * 
 * @returns uint32 -- String as uint32.
 */
template <>
auto ym::Ops::castTo<ym::uint32>(std::string const & S,
                                 uint32      const   Base) -> uint32
{
   auto val_uint64 = 0_u64;

   try
   {
      auto const Val = std::stoul(S, nullptr, Base);
      static_assert(std::is_same_v<decltype(Val), uint64 const>, "Unexpected type");
      val_uint64 = Val;
   }
   catch (std::exception const & E)
   {
      OpsError_BadCast::check(false, "String '%s' invalid or out of range uint64 (%s)", S.c_str(), E.what());
   }

   OpsError_BadCast::check(
      val_uint64 >= static_cast<uint64>(std::numeric_limits<uint32>::min()) &&
      val_uint64 <= static_cast<uint64>(std::numeric_limits<uint32>::max()),
      "String '%s' out of uint32 range", S.c_str());

   return static_cast<uint32>(val_uint64);
}

/** castTo
 *
 * @brief Casts string to uint64.
 * 
 * @note std::strtoul doesn't flag an error if conversion is invalid.
 * 
 * @note Most strings will likely be small enough for small string
 *       optimization to take effect.
 * 
 * @throws OpsError_BadCast -- If string is not a valid or out of range uint64.
 * 
 * @param S    -- String to cast.
 * @param Base -- Radix of uint64.
 * 
 * @returns uint64 -- String as uint64.
 */
template <>
auto ym::Ops::castTo<ym::uint64>(std::string const & S,
                                 uint32      const   Base) -> uint64
{
   auto val_uint64 = 0_u64;

   try
   {
      auto const Val = std::stoul(S, nullptr, Base);
      static_assert(std::is_same_v<decltype(Val), uint64 const>, "Unexpected type");
      val_uint64 = Val;
   }
   catch (std::exception const & E)
   {
      OpsError_BadCast::check(false, "String '%s' invalid or out of range uint64 (%s)", S.c_str(), E.what());
   }

   return val_uint64;
}

/** castTo
 *
 * @brief Casts string to float32.
 * 
 * @note std::strtof doesn't flag an error if conversion is invalid.
 * 
 * @note Most strings will likely be small enough for small string
 *       optimization to take effect.
 * 
 * @throws OpsError_BadCast -- If string is not a valid or out of range float32.
 * 
 * @param S -- String to cast.
 * 
 * @returns float32 -- String as float32.
 */
template <>
auto ym::Ops::castTo<ym::float32>(std::string const & S) -> float32
{
   auto val_float32 = 0_f32;

   try
   {
      auto const Val = std::stof(S, nullptr);
      static_assert(std::is_same_v<decltype(Val), float32 const>, "Unexpected type");
      val_float32 = Val;
   }
   catch (std::exception const & E)
   {
      OpsError_BadCast::check(false, "String '%s' invalid or out of range float32 (%s)", S.c_str(), E.what());
   }

   return val_float32;
}

/** castTo
 *
 * @brief Casts string to float64.
 * 
 * @note std::strtof doesn't flag an error if conversion is invalid.
 * 
 * @note Most strings will likely be small enough for small string
 *       optimization to take effect.
 * 
 * @throws OpsError_BadCast -- If string is not a valid or out of range float64.
 * 
 * @param S -- String to cast.
 * 
 * @returns float64 -- String as float64.
 */
template <>
auto ym::Ops::castTo<ym::float64>(std::string const & S) -> float64
{
   auto val_float64 = 0_f64;

   try
   {
      auto const Val = std::stod(S, nullptr);
      static_assert(std::is_same_v<decltype(Val), float64 const>, "Unexpected type");
      val_float64 = Val;
   }
   catch (std::exception const & E)
   {
      OpsError_BadCast::check(false, "String '%s' invalid or out of range float64 (%s)", S.c_str(), E.what());
   }

   return val_float64;
}

/** castTo
 *
 * @brief Casts string to float80.
 * 
 * @note std::strtof doesn't flag an error if conversion is invalid.
 * 
 * @note Most strings will likely be small enough for small string
 *       optimization to take effect.
 * 
 * @throws OpsError_BadCast -- If string is not a valid or out of range float80.
 * 
 * @param S -- String to cast.
 * 
 * @returns float80 -- String as float80.
 */
template <>
auto ym::Ops::castTo<ym::float80>(std::string const & S) -> float80
{
   auto val_float80 = 0_f80;

   try
   {
      auto const Val = std::stold(S, nullptr);
      static_assert(std::is_same_v<decltype(Val), float80 const>, "Unexpected type");
      val_float80 = Val;
   }
   catch (std::exception const & E)
   {
      OpsError_BadCast::check(false, "String '%s' invalid or out of range float80 (%s)", S.c_str(), E.what());
   }

   return val_float80;
}
