/**
 * @file    rng.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include <array>
#include <limits>
#include <type_traits>
#include <x86intrin.h>

namespace ym
{

/** Randomable
 * 
 * @brief Supported types the PRNG is able to generate.
 * 
 * @tparam T -- Data type.
 */
template <typename T>
concept Randomable = std::is_same_v<T, uint32 > ||
                     std::is_same_v<T, uint64 > ||
                     std::is_same_v<T, float32> ||
                     std::is_same_v<T, float64>;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/** Prng
 * 
 * @brief A pseudo-random number generator.
 * 
 * @note Satisfies requirements of UniformRandomBitGenerator
 * 
 * @ref <https://en.cppreference.com/w/cpp/named_req/UniformRandomBitGenerator>
 * @ref <https://www.pcg-random.org/paper.html>
 * @ref <https://en.wikipedia.org/wiki/Linear-feedback_shift_register>.
 */
class Prng
{
public:
   friend class Trng;

   using result_type = uint64;
   using State_T     = uint64;

   explicit Prng(void);
   explicit Prng(State_T const Seed);

   void setSeed(State_T const Seed);

   inline auto getSeed (void) const { return _seed;  }
   inline auto getState(void) const { return _state; }

   template <Randomable Randomable_T>
   inline Randomable_T gen(void);

   void jump(uint64 nJumps);

   constexpr auto min(void) const { return std::numeric_limits<result_type>::min(); }
   constexpr auto max(void) const { return std::numeric_limits<result_type>::max(); }

   inline result_type operator () (void);

private:
   static inline float32 convertToFloat32(uint32 const Val);
   static inline float64 convertToFloat64(uint64 const Val);

   static constexpr auto _s_Mult = 6364136223846793005_u64;
   static constexpr auto _s_Plus = 1442695040888963407_u64;

   State_T _seed;
   State_T _state;
};

/** gen
 * 
 * @brief Generates uniform positive integer values in the range [0..2^64).
 * 
 * @note Changes internal state when called.
 *
 * @returns uint64 -- Random number in range.
 */
template <>
inline auto Prng::gen<uint64>(void) -> uint64
{
   // the lcg transform
   _state = (_state * _s_Mult) + _s_Plus;

   // the post-process transform
   auto const Word = ((_state >> ((_state >> 59_u64) + 5_u64)) ^ _state) * 12605985483714917081_u64;
   return (Word >> 43_u64) ^ Word;
}

/** gen
 * 
 * @brief Generates uniform positive integer values in the range [0..2^32).
 * 
 * @note Changes internal state when called.
 *
 * @returns uint32 -- Random number in range.
 */
template <>
inline auto Prng::gen<uint32>(void) -> uint32
{
   return static_cast<uint32>(gen<uint64>());
}

/** gen
 * 
 * @brief Generates uniform real values in the range [0..1).
 *        Resolution is 2^23 (~8 million).
 * 
 * @note Changes internal state when called.
 *
 * @returns float32 -- Random number in range.
 */
template <>
inline auto Prng::gen<float32>(void) -> float32
{
   return convertToFloat32(gen<uint32>());
}

/** gen
 * 
 * @brief Generates uniform real values in the range [0..1).
 *        Resolution is 2^52 (~4 quadrillion).
 * 
 * @note Changes internal state when called.
 *
 * @returns float64 -- Random number in range.
 */
template <>
inline auto Prng::gen<float64>(void) -> float64
{
   return convertToFloat64(gen<uint64>());
}

/** operator ()
 * 
 * @brief Generates uniform positive integer values in the range of result_type.
 * 
 * @note Changes internal state when called.
 *
 * @returns result_type -- Random number in range.
 */
inline auto Prng::operator () (void) -> result_type
{
   return gen<uint64>();
}

/** convertToFloat32
 * 
 * @brief Generates uniform real values in the range [0..1).
 *        Resolution is 2^23 (~8 million).
 * 
 * @param Val -- Value to convert.
 *
 * @returns float32 -- Random number in range.
 */
inline auto ym::Prng::convertToFloat32(uint32 const Val) -> float32
{
   union
   {
      uint32  u32;
      float32 f32;
   } uf{.u32 = Val};

   // sign bit  1
   // exp  bits 8
   // man  bits 23

   uf.u32  = uf.u32 >> (32_u32 - 23_u32); // move highest bits into mantissa
   uf.u32 |= 127_u32 << 23_u32; // bias exponent
   uf.f32 -= 1.0_f32;

   return uf.f32;
}

/** convertToFloat64
 * 
 * @brief Generates uniform real values in the range [0..1).
 *        Resolution is 2^52 (~4 quadrillion).
 * 
 * @param Val -- Value to convert.
 *
 * @returns float64 -- Random number in range.
 */
inline auto ym::Prng::convertToFloat64(uint64 const Val) -> float64
{
   union
   {
      uint64  u64;
      float64 f64;
   } uf{.u64 = Val};

   // sign bit  1
   // exp  bits 11
   // man  bits 52

   uf.u64  = uf.u64 >> (64_u64 - 52_u64); // move highest bits into mantissa
   uf.u64 |= 1023_u64 << 52_u64; // bias exponent
   uf.f64 -= 1.0_f64;

   return uf.f64;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/** Trng
 * 
 * @brief A true-random number generator.
 * 
 * @note Satisfies requirements of UniformRandomBitGenerator
 * 
 * @ref <https://en.cppreference.com/w/cpp/named_req/UniformRandomBitGenerator>
 */
class Trng
{
public:
   using result_type = uint64;

   explicit Trng(void);
   explicit Trng(Prng::State_T const Seed);

   inline auto const * getInternalPrngPtr(void) const { return &_prng; }

   template <Randomable Randomable_T>
   inline Randomable_T gen(void);

   constexpr auto min(void) const { return std::numeric_limits<result_type>::min(); }
   constexpr auto max(void) const { return std::numeric_limits<result_type>::max(); }

   inline result_type operator () (void);

private:
   Prng _prng;
};

/** gen
 * 
 * @brief Generates uniform positive integer values in the range [0..2^64).
 * 
 * @note Changes internal state when called.
 * 
 * @note __rdtsc() returns the cpu cycle count. @ref <https://learn.microsoft.com/en-us/cpp/intrinsics/rdtsc?view=msvc-170>
 *
 * @returns uint64 -- Random number in range.
 */
template <>
inline uint64 Trng::gen<uint64>(void)
{
   return _prng.gen<uint64>() ^ __rdtsc();
}

/** gen
 * 
 * @brief Generates uniform positive integer values in the range [0..2^32).
 * 
 * @note Changes internal state when called.
 *
 * @returns uint32 -- Random number in range.
 */
template <>
inline uint32 Trng::gen<uint32>(void)
{
   return static_cast<uint32>(gen<uint64>());
}

/** gen
 * 
 * @brief Generates uniform real values in the range [0..1).
 *        Resolution is 2^23 (~8 million).
 * 
 * @note Changes internal state when called.
 *
 * @returns float32 -- Random number in range.
 */
template <>
inline auto Trng::gen<float32>(void) -> float32
{
   return Prng::convertToFloat32(gen<uint32>());
}

/** gen
 * 
 * @brief Generates uniform real values in the range [0..1).
 *        Resolution is 2^52 (~4 quadrillion).
 * 
 * @note Changes internal state when called.
 *
 * @returns float64 -- Random number in range.
 */
template <>
inline auto Trng::gen<float64>(void) -> float64
{
   return Prng::convertToFloat64(gen<uint64>());
}

/** operator ()
 * 
 * @brief Generates uniform positive integer values in the range of result_type.
 * 
 * @note Changes internal state when called.
 *
 * @returns result_type -- Random number in range.
 */
inline auto Trng::operator () (void) -> result_type
{
   return gen<uint64>();
}

} // ym
