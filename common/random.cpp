/**
 * @author Forrest Jablonski
 */

#include "random.h"

/**
 * Seed got by
 * $ dd if=/dev/urandom of=./random.bytes bs=4 count=4
 * $ hexdump random.bytes
 */
ym::Random::Random(void)
   : _s0 {0x1723'73c3'5cc2'77fbull},
     _s1 {0x8f51'e36d'13fa'4f21ull},
     _s2 {0x0c60'dc05'b8f9'266aull},
     _s3 {0xea9b'fe26'838f'091dull}
{
}

/**
 * 
 */
inline void ym::Random::gen_helper(void)
{
   auto const Tmp = _s1 << 17;

   _s2 ^= _s0;
   _s3 ^= _s1;
   _s1 ^= _s2;
   _s0 ^= _s3;

   _s2 ^= Tmp;

   _s3 ^= (_s3 << 45) | (_s3 >> (64 - 45)); // rotate left
}

/**
 * <https://prng.di.unimi.it/>
 *
 * xoshiro256+
 *
 * generates uniform positive integer values in the range [0..2^32)
 */
template <>
auto ym::Random::gen<ym::uint32>(void) -> uint32
{
   uint32 const Result = static_cast<uint32>((_s0 + _s3) >> 32ull);

   gen_helper();

   return Result;
}

/**
 * <https://prng.di.unimi.it/>
 *
 * xoshiro256+
 *
 * generates uniform positive integer values in the range [0..2^64)
 */
template <>
auto ym::Random::gen<ym::uint64>(void) -> uint64
{
   uint64 const Result = _s0 + _s3;

   gen_helper();

   return Result;
}

/**
 * <https://prng.di.unimi.it/>
 *
 * xoshiro256+
 *
 * generates uniform real values in the range [0..1)
 * resolution is 2^23 (~8 million)
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

   uf.u32  = uf.u64 >> (64 - 23); // move highest bits into mantissa
   uf.u32 |= 127u << 23; // bias exponent
   uf.f32 -= 1.0f;

   return uf.f32;
}

/**
 * <https://prng.di.unimi.it/>
 *
 * xoshiro256+
 *
 * generates uniform real values in the range [0..1)
 * resolution is 2^52 (~4 quadrillion)
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

   uf.u64  = uf.u64 >> (64 - 52); // move highest bits into mantissa
   uf.u64 |= 1023ull << 52; // bias exponent
   uf.f64 -= 1.0;

   return uf.f64;
}

/**
 * A jump is equivalent to 2^128 calls to gen()
 */
void ym::Random::jump(uint32 const NJumps)
{
   constexpr uint64 JumpTable[] = {
      0x180ec6d33cfd0abaull,
      0xd5a61266f0c9392cull,
      0xa9582618e03fc9aaull,
      0x39abdc4529b1661cull
   };
   constexpr uint32 JumpTableSize = sizeof(JumpTable) / sizeof(*JumpTable);

   for (uint32 i = 0; i < NJumps; ++i)
   {
      uint64 s0 = 0;
      uint64 s1 = 0;
      uint64 s2 = 0;
      uint64 s3 = 0;

      for (uint32 j = 0; j < JumpTableSize; ++j)
      {
         for (uint32 k = 0; k < 64; ++k)
         {
            if (JumpTable[j] & (1ull << k))
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
