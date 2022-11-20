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
   : _seed  {0x1723'73c3'5cc2'77fb_u64,
             0x8f51'e36d'13fa'4f21_u64,
             0x0c60'dc05'b8f9'266a_u64,
             0xea9b'fe26'838f'091d_u64},

     _state {_seed[0],
             _seed[1],
             _seed[2],
             _seed[3]}
{
}

/** seed
 * 
 * @brief Sets the seed for the PRNG.
 * 
 * @param Seed -- Seed.
 * 
 * @note Seed cannot be 0 everywhere.
 */
void ym::Random::seed(uint64 const Seed[4])
{
   if (!(Seed[0] == 0_u64 &&
         Seed[1] == 0_u64 &&
         Seed[2] == 0_u64 &&
         Seed[3] == 0_u64))
   { // not zero everywhere - seed is accepted
      _state[0] = _seed[0] = Seed[0];
      _state[1] = _seed[1] = Seed[1];
      _state[2] = _seed[2] = Seed[2];
      _state[3] = _seed[3] = Seed[3];
   }
}

/** getSeed
 * 
 * @brief Returns the seed through the out pointer.
 * 
 * @param seed_out -- Storage to populate the seed in.
 */
void ym::Random::getSeed(uint64 seed_out[4])
{
   seed_out[0] = _seed[0];
   seed_out[1] = _seed[1];
   seed_out[2] = _seed[2];
   seed_out[3] = _seed[3];
}

/** getState
 * 
 * @brief Returns the state through the out pointer.
 * 
 * @param seed_out -- Storage to populate the state in.
 */
void ym::Random::getState(uint64 state_out[4])
{
   state_out[0] = _state[0];
   state_out[1] = _state[1];
   state_out[2] = _state[2];
   state_out[3] = _state[3];
}

/** gen_helper
 * 
 * @brief Core implementation of PRNG.
 */
inline void ym::Random::gen_helper(void)
{
   auto const Tmp = _state[1] << 17_u64;

   _state[2] ^= _state[0];
   _state[3] ^= _state[1];
   _state[1] ^= _state[2];
   _state[0] ^= _state[3];

   _state[2] ^= Tmp;

   _state[3] ^= (_state[3] << 45_u64) | (_state[3] >> (64_u64 - 45_u64)); // rotate left
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
   uint32 const Result = static_cast<uint32>((_state[0] + _state[3]) >> 32_u64);

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
   uint64 const Result = _state[0] + _state[3];

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
   } uf{.u64 = _state[0] + _state[3]};

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
   } uf{.u64 = _state[0] + _state[3]};

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
      auto s0 = 0_u64;
      auto s1 = 0_u64;
      auto s2 = 0_u64;
      auto s3 = 0_u64;

      for (auto j = 0_u64; j < JumpTable.size(); ++j)
      {
         for (auto k = 0_u64; k < 64_u64; ++k)
         {
            if (JumpTable[j] & (1_u64 << k))
            {
               s0 ^= _state[0];
               s1 ^= _state[1];
               s2 ^= _state[2];
               s3 ^= _state[3];
            }

            gen_helper();
         }
      }

      _state[0] = s0;
      _state[1] = s1;
      _state[2] = s2;
      _state[3] = s3;
   }
}
