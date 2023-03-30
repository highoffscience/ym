/**
 * @file    random.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include <array>
#include <type_traits>

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

/** Random
 * 
 * @brief A pseudo-random number generator.
 * 
 * @ref <https://prng.di.unimi.it/>.
 * @ref <https://en.wikipedia.org/wiki/Linear-feedback_shift_register>.
 */
class Random
{
public:
   using State_T = std::array<uint64, 4>;

   explicit Random(void);
   explicit Random(State_T const Seed);

   static bool isZero(State_T const State);

   void setSeed(State_T const Seed);

   inline auto getSeed (void) const { return _seed;  }
   inline auto getState(void) const { return _state; }

   template <Randomable Randomable_T>
   Randomable_T gen(void);

   void jump(uint32 const NJumps);

private:
   inline void gen_helper(void);

   State_T _seed;
   State_T _state;
};

template <> uint32  Random::gen<uint32 >(void);
template <> uint64  Random::gen<uint64 >(void);

template <> float32 Random::gen<float32>(void);
template <> float64 Random::gen<float64>(void);

} // ym
