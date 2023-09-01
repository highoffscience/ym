/**
 * @file    prng.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "prng.h"

/** PRNG
 * 
 * @brief Constructor.
 */
ym::PRNG::PRNG(void)
   : PRNG({0_u64, 0_u64, 0_u64, 0_u64})
{
}

/** PRNG
 * 
 * @brief Constructor.
 * 
 * @param Seed -- Seed.
 */
ym::PRNG::PRNG(State_T const Seed)
   : _seed  {Seed},
     _state {Seed}
{
   setSeed(Seed); // some prngs states cannot be 0 - always call this to verify
}

/** setSeed
 * 
 * @brief Sets the seed for the PRNG.
 * 
 * @param Seed -- Seed.
 * 
 * @note Seed cannot be 0 everywhere.
 * 
 * @note Default seed obtained by
 *       $ dd if=/dev/urandom of=./random.bytes bs=4 count=4
 *       $ hexdump random.bytes
 */
void ym::PRNG::setSeed(State_T const Seed)
{
   auto const IsZeroState =
      Seed[0] == 0_u64 &&
      Seed[1] == 0_u64 &&
      Seed[2] == 0_u64 &&
      Seed[3] == 0_u64;

   if (IsZeroState)
   { // zero state not allowed - set to default seed
      setSeed({0x1723'73c3'5cc2'77fb_u64,
               0x8f51'e36d'13fa'4f21_u64,
               0x0c60'dc05'b8f9'266a_u64,
               0xea9b'fe26'838f'091d_u64});
   }
   else
   { // not zero everywhere - seed is accepted
      _seed  = Seed;
      _state = Seed;
   }
}

/** gen
 * 
 * @brief Generates uniform positive integer values in the range [0..2^32).
 * 
 * @note Changes internal state when called.
 * 
 * @note xoshiro256++ - @ref <https://prng.di.unimi.it/>.
 *
 * @returns uint32 -- Random number in range.
 */
template <>
auto ym::PRNG::gen<ym::uint32>(void) -> uint32
{
   auto const Result =
      static_cast<uint32>(
         (rotl(_state[0] + _state[3], 23_u64) + _state[0]) >> 32_u32);

   gen_helper();

   return Result;
}

/** gen
 * 
 * @brief Generates uniform positive integer values in the range [0..2^64).
 * 
 * @note Changes internal state when called.
 * 
 * @note xoshiro256++ - @ref <https://prng.di.unimi.it/>.
 *
 * @returns uint64 -- Random number in range.
 */
template <>
auto ym::PRNG::gen<ym::uint64>(void) -> uint64
{
   auto const Result = rotl(_state[0] + _state[3], 23_u64) + _state[0];

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
 * @note xoshiro256++ - @ref <https://prng.di.unimi.it/>.
 *
 * @returns float32 -- Random number in range.
 */
template <>
auto ym::PRNG::gen<ym::float32>(void) -> float32
{
   union
   {
      uint32  u32;
      float32 f32;
   } uf{.u32 = gen<uint32>()};

   // sign bit  1
   // exp  bits 8
   // man  bits 23

   uf.u32  = uf.u32 >> (32_u32 - 23_u32); // move highest bits into mantissa
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
 * @note xoshiro256++ - @ref <https://prng.di.unimi.it/>.
 *
 * @returns float64 -- Random number in range.
 */
template <>
auto ym::PRNG::gen<ym::float64>(void) -> float64
{
   union
   {
      uint64  u64;
      float64 f64;
   } uf{.u64 = gen<uint64>()};

   // sign bit  1
   // exp  bits 11
   // man  bits 52

   uf.u64  = uf.u64 >> (64_u64 - 52_u64); // move highest bits into mantissa
   uf.u64 |= 1023_u64 << 52_u64; // bias exponent
   uf.f64 -= 1.0_f64;

   return uf.f64;
}

/** jump
 * 
 * @brief Advances the state by 2^128 calls to gen().
 * 
 * @note xoshiro256++ - @ref <https://prng.di.unimi.it/>.
 * 
 * @param NJumps -- # of jumps to perform.
 */
void ym::PRNG::jump(uint32 const NJumps)
{
   constexpr std::array JumpTable{
      0x180ec6d33cfd0aba_u64,
      0xd5a61266f0c9392c_u64,
      0xa9582618e03fc9aa_u64,
      0x39abdc4529b1661c_u64
   };

   for (auto i = 0_u32; i < NJumps; ++i)
   {
      State_T s{0_u64, 0_u64, 0_u64, 0_u64};

      for (auto j = 0_u64; j < JumpTable.size(); ++j)
      {
         for (auto k = 0_u64; k < 64_u64; ++k)
         {
            if (JumpTable[j] & (1_u64 << k))
            {
               s[0] ^= _state[0];
               s[1] ^= _state[1];
               s[2] ^= _state[2];
               s[3] ^= _state[3];
            }

            gen_helper();
         }
      }

      _state[0] = s[0];
      _state[1] = s[1];
      _state[2] = s[2];
      _state[3] = s[3];
   }
}

/** gen_helper
 * 
 * @brief Core implementation of PRNG.
 * 
 * @note xoshiro256++ - @ref <https://prng.di.unimi.it/>.
 */
inline void ym::PRNG::gen_helper(void)
{
   auto const Tmp = _state[1] << 17_u64;

   _state[2] ^= _state[0];
   _state[3] ^= _state[1];
   _state[1] ^= _state[2];
   _state[0] ^= _state[3];

   _state[2] ^= Tmp;

   _state[3] ^= rotl(_state[3], 45_u64);
}

/** rotl
 * 
 * @brief Rotates 64 bit integer to the left by specified amount.
 * 
 * @param X           -- Value to rotate.
 * @param ShiftAmount -- Amount to rotate by.
 * 
 * @returns uint64 -- The rotated value.
 */
inline auto ym::PRNG::rotl(uint64 const X,
                           uint64 const ShiftAmount) const -> uint64
{
   return (X << ShiftAmount) | (X >> (64_u64 - ShiftAmount));
}
