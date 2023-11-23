/**
 * @file    prng.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include <array>
#include <limits>
#include <type_traits>

namespace ym
{

/** Randomable_T
 * 
 * @brief Supported types the PRNG is able to generate.
 * 
 * @tparam T -- Data type.
 */
template <typename T>
concept Randomable_T = std::is_same_v<T, uint32 > ||
                       std::is_same_v<T, uint64 > ||
                       std::is_same_v<T, float32> ||
                       std::is_same_v<T, float64>;

/** Random
 * 
 * @brief A pseudo-random number generator.
 * 
 * @note Satisfies requirements of UniformRandomBitGenerator
 * 
 * @ref <https://en.cppreference.com/w/cpp/named_req/UniformRandomBitGenerator>
 * @ref <https://prng.di.unimi.it/>.
 * @ref <https://en.wikipedia.org/wiki/Linear-feedback_shift_register>.
 */
class PRNG
{
public:
   using result_type = uint64;
   using State_T     = std::array<result_type, 4>;

   explicit PRNG(void);
   explicit PRNG(State_T const Seed);

   void setSeed(State_T const Seed);

   inline auto getSeed (void) const { return _seed;  }
   inline auto getState(void) const { return _state; }

   template <Randomable_T Randomable>
   Randomable gen(void);

   void jump(uint32 const NJumps = 1_u32);

   constexpr auto min(void) const { return std::numeric_limits<result_type>::min(); }
   constexpr auto max(void) const { return std::numeric_limits<result_type>::max(); }

   inline auto operator () (void);

private:
   inline void gen_helper(void);

   inline uint64 rotl(uint64 const X,
                      uint64 const ShiftAmount) const;

   State_T _seed;
   State_T _state;
};

template <> uint32  PRNG::gen<uint32 >(void);
template <> uint64  PRNG::gen<uint64 >(void);

template <> float32 PRNG::gen<float32>(void);
template <> float64 PRNG::gen<float64>(void);

inline auto PRNG::operator () (void) { return gen<uint64>(); }

} // ym
