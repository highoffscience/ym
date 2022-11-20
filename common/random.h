/**
 * @file    random.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

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
   explicit Random(void);

   void seed(uint64 const Seed[4]);

   void getSeed (uint64  seed_out[4]);
   void getState(uint64 state_out[4]);

   template <Randomable Randomable_T>
   Randomable_T gen(void);

   void jump(uint32 const NJumps);

private:
   inline void gen_helper(void);

   uint64 _seed [4];
   uint64 _state[4];
};

template <> uint32  Random::gen<uint32 >(void);
template <> uint64  Random::gen<uint64 >(void);

template <> float32 Random::gen<float32>(void);
template <> float64 Random::gen<float64>(void);

} // ym
