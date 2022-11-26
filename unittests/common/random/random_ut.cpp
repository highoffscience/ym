/**
 * @file    random_ut.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ym.h"
#include "random_ut.h"

#include "random.h"

#include <bitset>
#include <cstdio>
#include <vector>

/** Random_UT
 *
 * @brief Constructor.
 */
ym::ut::Random_UT::Random_UT(void)
   : UnitTestBase("Random", {"MemoryManager"})
{
}

/** run
 *
 * @brief Runs zeros and ones test.
 * 
 * @note Amount of zeros and ones should be about equal.
 *
 * @return bool -- True if test passed, false otherwise.
 */
bool ym::ut::Random_UT::ZerosAndOnes_TC::run(void)
{
   ym::Random rand;

   constexpr auto NIters   = 1'000'000'000_u64;
             auto nSetBits = 0_u64;

   for (auto i = 0_u64; i < NIters; ++i)
   {
      nSetBits += std::bitset<64>(rand.gen<uint64>()).count();
   }

   std::printf("NIters   %lu\n", NIters);
   std::printf("NSetBits %lu\n", nSetBits);
   std::printf("Ratio    %lf\n", static_cast<float64>(nSetBits) /
                                 static_cast<float64>(NIters));

   return false; // TODO
}

/** run
 *
 * @brief Runs uniform bins test.
 * 
 * @note Amount of entries in each bin should be about equal.
 *
 * @return bool -- True if test passed, false otherwise.
 */
bool ym::ut::Random_UT::UniformBins_TC::run(void)
{
   ym::Random rand;

   std::vector histo(1_u64 << 16_u64, 0_u64);

   constexpr auto NIters = 1'000'000'000_u64;

   for (auto i = 0_u64; i < NIters; ++i)
   {
      auto const R = rand.gen<uint64>();

      histo[(R >> 48_u64) & 0xffff_u64] += 1_u64;
      histo[(R >> 32_u64) & 0xffff_u64] += 1_u64;
      histo[(R >> 16_u64) & 0xffff_u64] += 1_u64;
      histo[(R >>  0_u64) & 0xffff_u64] += 1_u64;
   }

   auto const NExpectedHitsPerBin = NIters / histo.size();
}

/** run
 *
 * @brief Runs the bin frequency test.
 * 
 * @todo I want to use ym::MemMan, add dependency for that test to run first.
 *
 * @return bool -- True if test passed, false otherwise.
 */
bool ym::ut::Random_UT::BinFrequency_TC::run(void)
{
   ym::Random rand;

   return rand.gen<ym::uint32>() % 2 == 0;
}
