/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <type_traits>

namespace ym
{

/** Random
 * 
 * @brief PRNG 
 * 
 * @todo You are using the link annotation incorrectly Forrest
 * 
 * @note If modifying the implementation don't forget to update
 *       < @link rng_ym_rand.h @endlink > in the dieharder test suite.
 */
class Random
{
public:
   explicit Random(void);

   void seed(uint64 const S0,
             uint64 const S1,
             uint64 const S2,
             uint64 const S3);

   template <typename T>
   requires(std::is_same_v<T, uint32 > ||
            std::is_same_v<T, uint64 > ||
            std::is_same_v<T, float32> ||
            std::is_same_v<T, float64>)
   T gen(void);

   void jump(uint32 const NJumps);

private:
   inline void gen_helper(void);

   uint64 _s0;
   uint64 _s1;
   uint64 _s2;
   uint64 _s3;
};

template <> uint32  Random::gen<uint32 >(void);
template <> uint64  Random::gen<uint64 >(void);

template <> float32 Random::gen<float32>(void);
template <> float64 Random::gen<float64>(void);

} // ym
