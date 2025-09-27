/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"
#include "testsuite.h"

#include "rng.h"
#include "textlogger.h"
#include "ymerror.h"

#include <algorithm>
#include <bitset>
#include <utility>
#include <vector>

/** TestSuite
 *
 * @brief Constructor.
 */
ym::unit::TestSuite::TestSuite(void)
   : TestSuiteBase("Rng")
{
   addTestCase<ZerosAndOnes>();
   addTestCase<UniformBins >();
}

/** run
 *
 * @brief Runs zeros and ones test.
 * 
 * @note Amount of zeros and ones should be about equal.
 * 
 * @note That's what a relationship is. We average our misery.
 *
 * @returns DataShuttle -- Important values acquired during run of test case.
 */
auto ym::unit::TestSuite::ZerosAndOnes::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_Rng);

   ym::Prng rand;

   std::vector<std::pair<uint64, uint64>> setBitVector;

   for (auto i = 8_u64; i <= 24_u64; ++i)
   {
      auto const NIters   = (1_u64 << i);
      auto       nSetBits = 0_u64;
      for (auto j = 0_u64; j < NIters; ++j)
      {
         nSetBits += std::bitset<64>(rand.gen<uint64>()).count();
      }
      auto const NTrials = NIters * 64_u64;
      setBitVector.emplace_back(NTrials, std::min(nSetBits, NTrials - nSetBits));
   }

   return {
      {"SetBitVector", setBitVector}
   };
}

/** run
 *
 * @brief Runs uniform bins test.
 * 
 * @note Amount of entries in each bin should be about equal.
 *
 * @returns DataShuttle -- Important values acquired during run of test case.
 */
auto ym::unit::TestSuite::UniformBins::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_Rng);

   ym::Prng rand;

   constexpr auto BitShiftAmount = 16_u64;
   static_assert(BitShiftAmount <= 32_u64, "Must contain at most uint32 # of bits");

   std::vector<uint64> u32Bins(1_u64 << BitShiftAmount, 0_u64);
   std::vector<uint64> u64Bins(1_u64 << BitShiftAmount, 0_u64);
   std::vector<uint64> f32Bins(1_u64 << BitShiftAmount, 0_u64);
   std::vector<uint64> f64Bins(1_u64 << BitShiftAmount, 0_u64);

   constexpr auto NIters = 100'000_u64;

   for (auto i = 0_u64; i < NIters; ++i)
   {
      u32Bins[rand.gen<uint32>() >> (32_u32 - BitShiftAmount)] += 1_u64;
      u64Bins[rand.gen<uint64>() >> (64_u32 - BitShiftAmount)] += 1_u64;

      f32Bins[static_cast<uint64>(
         rand.gen<float32>() * static_cast<float32>(f32Bins.size()))] += 1_u64;
      f64Bins[static_cast<uint64>(
         rand.gen<float64>() * static_cast<float64>(f64Bins.size()))] += 1_u64;
   }

   return {
      {"u32Bins", u32Bins},
      {"u64Bins", u64Bins},
      {"f32Bins", f32Bins},
      {"f64Bins", f64Bins}
   };
}
