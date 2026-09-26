/**
 * @file    rng.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include <limits>
#include <type_traits>
#include <x86intrin.h>

namespace ym
{

/*
 * Defines:
 *    Prng class.
 *    Trng class.
 */

/**
 * @brief Supported types the PRNG is able to generate.
 *
 * @tparam T -- Data type.
 */
template <typename T>
concept Randomable =
   std::is_same_v<T, uint32 > ||
   std::is_same_v<T, uint64 > ||
   std::is_same_v<T, float32> ||
   std::is_same_v<T, float64>;

// -----------------------------------------------------------------------------

/**
 * @brief A pseudo-random number generator.
 *
 * - Satisfies requirements of [UniformRandomBitGenerator](https://en.cppreference.com/w/cpp/named_req/UniformRandomBitGenerator).
 * - Implementation based on [PCG Algorithms](https://www.pcg-random.org/paper.html).
 * - Theory behind implementation found in the [Wiki](https://en.wikipedia.org/wiki/Linear-feedback_shift_register).
 *
 * __Unit Test__
 * - @ref ym::unit::rng::TestSuite::Class_Prng.
 *
 * @test Shall ?
 */
class Prng
{
public:
   friend class Trng; // access to convertToFloatXX()

   using result_type = uint64;
   using State_T     = result_type;

   /**
    * @{
    * @brief Constructor.
    *    1. Default constructor.
    *    2. Seed initial generator.
    *
    * @param Seed -- (2) Seed.
    */
   explicit inline Prng(void) noexcept :
      Prng(0x763b'15c2'1847'ea8d_u64) // some hard-coded random number
   { }

   explicit inline Prng(State_T const Seed) noexcept {
      setSeed(Seed);
   }
   /// @}

   void setSeed(State_T const Seed) noexcept;

   /**
    * @{
    * @brief Getters.
    *
    * @returns auto -- Seed or state value.
    */
   inline auto getSeed (void) const noexcept { return _seed;  }
   inline auto getState(void) const noexcept { return _state; }
   /// @}

   template <Randomable Randomable_T>
   inline Randomable_T gen(void) noexcept;

   void jump(uint64 nJumps) noexcept;

   /**
    * @{
    * @brief Ranges for possible generator output.
    *
    * @returns auto -- Min or max generated value.
    */
   constexpr auto min(void) const noexcept { return std::numeric_limits<result_type>::min(); }
   constexpr auto max(void) const noexcept { return std::numeric_limits<result_type>::max(); }
   /// @}

   inline result_type operator () (void) noexcept;

private:
   static inline float32 convertToFloat32(uint32 const Val) noexcept;
   static inline float64 convertToFloat64(uint64 const Val) noexcept;

   static constexpr auto _s_Mult = 6364136223846793005_u64;
   static constexpr auto _s_Plus = 1442695040888963407_u64;

   State_T _seed  {0_u64};
   State_T _state {0_u64};
};

/**
 * @brief Generates uniform positive integer values in the range [0..2^64).
 *
 * - Changes internal state when called.
 *
 * @returns uint64 -- Random number in range.
 */
template <>
inline auto Prng::gen<uint64>(void) noexcept -> uint64
{
   // the lcg transform
   _state = (_state * _s_Mult) + _s_Plus;

   // the post-process transform
   auto const Word = ((_state >> ((_state >> 59_u64) + 5_u64)) ^ _state) * 12605985483714917081_u64;
   return (Word >> 43_u64) ^ Word;
}

/**
 * @brief Generates uniform positive integer values in the range [0..2^32).
 *
 * - Changes internal state when called.
 *
 * @returns uint32 -- Random number in range.
 */
template <>
inline auto Prng::gen<uint32>(void) noexcept -> uint32
{
   return static_cast<uint32>(gen<uint64>());
}

/**
 * @brief Generates uniform real values in the range [0..1). Resolution is 2^23 (~8 million).
 *
 * - Changes internal state when called.
 *
 * @returns float32 -- Random number in range.
 */
template <>
inline auto Prng::gen<float32>(void) noexcept -> float32
{
   return convertToFloat32(gen<uint32>());
}

/**
 * @brief Generates uniform real values in the range [0..1). Resolution is 2^52 (~4 quadrillion).
 *
 * - Changes internal state when called.
 *
 * @returns float64 -- Random number in range.
 */
template <>
inline auto Prng::gen<float64>(void) noexcept -> float64
{
   return convertToFloat64(gen<uint64>());
}

/**
 * @brief Generates uniform positive integer values in the range of result_type.
 *
 * - Changes internal state when called.
 *
 * @internal NOTE: Must be declared after the gen<>() functions.
 *
 * @returns result_type -- Random number in range.
 */
inline auto Prng::operator () (void) noexcept -> result_type
{
   return gen<result_type>();
}

/**
 * @brief Generates uniform real values in the range [0..1). Resolution is 2^23 (~8 million).
 *
 * @param Val -- Value to convert.
 *
 * @returns float32 -- Random number in range.
 */
inline auto ym::Prng::convertToFloat32(uint32 const Val) noexcept -> float32
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

/**
 * @brief Generates uniform real values in the range [0..1). Resolution is 2^52 (~4 quadrillion).
 *
 * @param Val -- Value to convert.
 *
 * @returns float64 -- Random number in range.
 */
inline auto ym::Prng::convertToFloat64(uint64 const Val) noexcept -> float64
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

/**
 * @brief A true-random number generator.
 *
 * - Satisfies requirements of [UniformRandomBitGenerator](https://en.cppreference.com/w/cpp/named_req/UniformRandomBitGenerator).
 *
 * __Unit Test__
 * - @ref ym::unit::rng::TestSuite::Class_Trng.
 *
 * @test Shall ?
 */
class Trng
{
public:
   using result_type = uint64;

   /**
    * @{
    * @brief Constructor.
    *    1. Default constructor.
    *    2. Seed initial generator.
    *
    * @param Seed -- (2) Seed.
    */
   explicit inline Trng(void) noexcept = default;
   explicit inline Trng(Prng::State_T const Seed) noexcept {
      _prng.setSeed(Seed);
   }
   /// @}

   /**
    * @brief Gets internal prng.
    *
    * @returns bound<Prng> -- Internal prng for this generator.
    */
   inline bound<Prng> getInternalPrng(void) noexcept {
      return {&_prng, ym_AssumePtrNotNull{}};
   }

   template <Randomable Randomable_T>
   inline Randomable_T gen(void) noexcept;

   inline result_type operator () (void) noexcept;

private:
   Prng _prng{};
};

/**
 * @brief Generates uniform positive integer values in the range [0..2^64).
 *
 * - Changes internal state when called.
 *
 * @note __rdtsc() returns the cpu cycle count.
 *    [Reference](https://learn.microsoft.com/en-us/cpp/intrinsics/rdtsc?view=msvc-170).
 *
 * @returns uint64 -- Random number in range.
 */
template <>
inline auto Trng::gen<uint64>(void) noexcept -> uint64
{
   return _prng.gen<uint64>() ^ __rdtsc();
}

/**
 * @brief Generates uniform positive integer values in the range [0..2^32).
 *
 * - Changes internal state when called.
 *
 * @returns uint32 -- Random number in range.
 */
template <>
inline auto Trng::gen<uint32>(void) noexcept -> uint32
{
   return static_cast<uint32>(gen<uint64>());
}

/**
 * @brief Generates uniform real values in the range [0..1). Resolution is 2^23 (~8 million).
 *
 * - Changes internal state when called.
 *
 * @returns float32 -- Random number in range.
 */
template <>
inline auto Trng::gen<float32>(void) noexcept -> float32
{
   return Prng::convertToFloat32(gen<uint32>());
}

/**
 * @brief Generates uniform real values in the range [0..1). Resolution is 2^52 (~4 quadrillion).
 *
 * - Changes internal state when called.
 *
 * @returns float64 -- Random number in range.
 */
template <>
inline auto Trng::gen<float64>(void) noexcept -> float64
{
   return Prng::convertToFloat64(gen<uint64>());
}

/**
 * @brief Generates uniform positive integer values in the range of result_type.
 *
 * - Changes internal state when called.
 *
 * @internal NOTE: Must be declared after the gen<>() functions.
 *
 * @returns result_type -- Random number in range.
 */
inline auto Trng::operator () (void) noexcept -> result_type
{
   return gen<uint64>();
}

} // ym
