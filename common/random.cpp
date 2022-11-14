/**
 * @file    random.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "random.h"

#include <array>

/** Random
 * 
 * @brief Constructor.
 * 
 * @note Seed got by
 *       $ dd if=/dev/urandom of=./random.bytes bs=4 count=4
 *       $ hexdump random.bytes
 */
ym::Random::Random(void)
   : _s0 {0x1723'73c3'5cc2'77fb_u64},
     _s1 {0x8f51'e36d'13fa'4f21_u64},
     _s2 {0x0c60'dc05'b8f9'266a_u64},
     _s3 {0xea9b'fe26'838f'091d_u64}
{
}

/** seed
 * 
 * @brief Sets the seed for the PRNG.
 * 
 * @param S0 -- 1st part of seed.
 * @param S1 -- 2nd part of seed.
 * @param S2 -- 3rd part of seed.
 * @param S3 -- 4th part of seed.
 * 
 * @note Seed cannot be 0 everywhere.
 */
void ym::Random::seed(uint64 const S0,
                      uint64 const S1,
                      uint64 const S2,
                      uint64 const S3)
{
   if (!(S0 == 0_u64 &&
         S1 == 0_u64 &&
         S2 == 0_u64 &&
         S3 == 0_u64))
   { // not zero everywhere - seed is accepted
      _s0 = S0;
      _s1 = S1;
      _s2 = S2;
      _s3 = S3;
   }
}

/** gen_helper
 * 
 * @brief Core implementation of PRNG.
 */
inline void ym::Random::gen_helper(void)
{
   auto const Tmp = _s1 << 17_u64;

   _s2 ^= _s0;
   _s3 ^= _s1;
   _s1 ^= _s2;
   _s0 ^= _s3;

   _s2 ^= Tmp;

   _s3 ^= (_s3 << 45_u64) | (_s3 >> (64_u64 - 45_u64)); // rotate left
}

/** gen
 * 
 * @brief Generates uniform positive integer values in the range [0..2^32).
 * 
 * @note Changes internal state when called.
 * 
 * @note xoshiro256+ - @ref <https://prng.di.unimi.it/>.
 *
 * @return Random number in range.
 */
template <>
auto ym::Random::gen<ym::uint32>(void) -> uint32
{
   uint32 const Result = static_cast<uint32>((_s0 + _s3) >> 32_u64);

   gen_helper();

   return Result;
}

/** gen
 * 
 * @brief Generates uniform positive integer values in the range [0..2^64).
 * 
 * @note Changes internal state when called.
 * 
 * @note xoshiro256+ - @ref <https://prng.di.unimi.it/>.
 *
 * @return Random number in range.
 */
template <>
auto ym::Random::gen<ym::uint64>(void) -> uint64
{
   uint64 const Result = _s0 + _s3;

   gen_helper();

   return Result;
}

/** gen
 * 
 * @brief Generates uniform real values in the range [0..1).
 *        Resolution is 2^23 (~8 million).
 * 
 * @note Changes internal state when called.
 * 
 * @note xoshiro256+ - @ref <https://prng.di.unimi.it/>.
 *
 * @return Random number in range.
 */
template <>
auto ym::Random::gen<ym::float32>(void) -> float32
{
   union
   {
      uint64  u64;
      uint32  u32;
      float32 f32;
   } uf{.u64 = _s0 + _s3};

   gen_helper();

   uf.u32  = static_cast<uint32>(uf.u64 >> (64_u64 - 23_u64)); // move highest bits into mantissa
   uf.u32 |= 127_u32 << 23_u32; // bias exponent
   uf.f32 -= 1.0_f32;

   return uf.f32;
}

/** gen
 * 
 * @brief Generates uniform real values in the range [0..1).
 *        Resolution is 2^52 (~4 quadrillion).
 * 
 * @note Changes internal state when called.
 * 
 * @note xoshiro256+ - @ref <https://prng.di.unimi.it/>.
 *
 * @return Random number in range.
 */
template <>
auto ym::Random::gen<ym::float64>(void) -> float64
{
   union
   {
      uint64  u64;
      float64 f64;
   } uf{.u64 = _s0 + _s3};

   gen_helper();

   uf.u64  = uf.u64 >> (64_u64 - 52_u64); // move highest bits into mantissa
   uf.u64 |= 1023_u64 << 52_u64; // bias exponent
   uf.f64 -= 1.0_f64;

   return uf.f64;
}

/** jump
 * 
 * @brief Advances the state by 2^128 calls to gen().
 * 
 * @note xoshiro256+ - @ref <https://prng.di.unimi.it/>.
 * 
 * @param NJumps -- # of jumps to perform.
 */
void ym::Random::jump(uint32 const NJumps)
{
   constexpr std::array JumpTable{
      0x180ec6d33cfd0aba_u64,
      0xd5a61266f0c9392c_u64,
      0xa9582618e03fc9aa_u64,
      0x39abdc4529b1661c_u64
   };

   for (auto i = 0_u32; i < NJumps; ++i)
   {
      decltype(_s0) s0 = 0_u64;
      decltype(_s1) s1 = 0_u64;
      decltype(_s2) s2 = 0_u64;
      decltype(_s3) s3 = 0_u64;

      for (auto j = 0_u64; j < JumpTable.size(); ++j)
      {
         for (auto k = 0_u64; k < 64_u64; ++k)
         {
            if (JumpTable[j] & (1_u64 << k))
            {
               s0 ^= _s0;
               s1 ^= _s1;
               s2 ^= _s2;
               s3 ^= _s3;
            }

            gen_helper();
         }
      }

      _s0 = s0;
      _s1 = s1;
      _s2 = s2;
      _s3 = s3;
   }
}
