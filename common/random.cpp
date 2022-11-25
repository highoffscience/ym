/**
 * @file    random.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "random.h"

/**
 * @brief Default (non-zero) state.
 */
#define YM_S0 0x1723'73c3'5cc2'77fb_u64
#define YM_S1 0x8f51'e36d'13fa'4f21_u64
#define YM_S2 0x0c60'dc05'b8f9'266a_u64
#define YM_S3 0xea9b'fe26'838f'091d_u64

/** Random
 * 
 * @brief Constructor.
 * 
 * @note Seed got by
 *       $ dd if=/dev/urandom of=./random.bytes bs=4 count=4
 *       $ hexdump random.bytes
 */
ym::Random::Random(void)
   : Random({YM_S0, YM_S1, YM_S2, YM_S3})
{
}

/** Random
 * 
 * @brief Constructor.
 * 
 * @param Seed -- Seed.
 */
ym::Random::Random(State_T const Seed)
   : _seed  {Seed[0],
             Seed[1],
             Seed[2],
             Seed[3]},

     _state {Seed[0],
             Seed[1],
             Seed[2],
             Seed[3]}
{
   if (isZero(_state))
   { // state is 0 - set to default
      setSeed({YM_S0, YM_S1, YM_S2, YM_S3});
   }
}

/** isZero
 * 
 * @brief Returns if the input state is zero everywhere.
 * 
 * @param State -- State.
 * 
 * @return bool -- True if input state is 0 everywhere, false otherwise.
 */
bool ym::Random::isZero(State_T const State)
{
   return State[0] == 0_u64 &&
          State[1] == 0_u64 &&
          State[2] == 0_u64 &&
          State[3] == 0_u64;
}

/** setSeed
 * 
 * @brief Sets the seed for the PRNG.
 * 
 * @param Seed -- Seed.
 * 
 * @note Seed cannot be 0 everywhere.
 */
void ym::Random::setSeed(State_T const Seed)
{
   if (!isZero(Seed))
   { // not zero everywhere - seed is accepted
      _state[0] = _seed[0] = Seed[0];
      _state[1] = _seed[1] = Seed[1];
      _state[2] = _seed[2] = Seed[2];
      _state[3] = _seed[3] = Seed[3];
   }
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
