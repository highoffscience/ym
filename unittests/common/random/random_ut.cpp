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

   constexpr auto NTrials    = 1'000_u64;
   constexpr auto NIters     = 1'000'000_u64;
   constexpr auto NTotalBits = NIters * 64_u64;
   auto nCumulativeSumBits   = 0_i64;
   auto nTimesPos            = 0_i64;

   for (auto i = 0_u64; i < NTrials; ++i)
   {
      auto nSetBits = 0_u64;
      for (auto j = 0_u64; j < NIters; ++j)
      {
         nSetBits += std::bitset<64>(rand.gen<uint64>()).count();
      }

      auto const Diff = static_cast<int64>(nSetBits) -
                        static_cast<int64>(NTotalBits / 2_u64);

      if (Diff >= 0_i64)
      {
         nTimesPos += 1;
      }

      nCumulativeSumBits += Diff;
   }

   std::printf("# bits in all trials  %lu\n", NTotalBits * NTrials);
   std::printf("nCumulativeSumBits    %ld\n", nCumulativeSumBits);
   std::printf("nTimesPos             %ld\n", nTimesPos);

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

   constexpr auto NIters = 1_u64 << 30_u64;

   for (auto i = 0_u64; i < NIters; ++i)
   {
      auto const R = rand.gen<uint64>();

      histo[(R >> 48_u64) & 0xffff_u64] += 1_u64;
      histo[(R >> 32_u64) & 0xffff_u64] += 1_u64;
      histo[(R >> 16_u64) & 0xffff_u64] += 1_u64;
      histo[(R >>  0_u64) & 0xffff_u64] += 1_u64;
   }

   auto const NExpectedHitsPerBin = NIters / histo.size();

   return false; // TODO
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
