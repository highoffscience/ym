/**
 * @file    random_ut.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ym.h"
#include "random_ut.h"

#include "random.h"

#include <bitset>

/** Random_UT
 *
 * @brief Constructor.
 */
ym::ut::Random_UT::Random_UT(void)
   : UnitTestBase("Random")
{
   addTestCase<ZerosAndOnes>();
}

/** run
 *
 * @brief Runs zeros and ones test.
 * 
 * @note Amount of zeros and ones should be about equal.
 *
 * @return bool -- True if test passed, false otherwise.
 */
auto ym::ut::Random_UT::ZerosAndOnes::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   ym::Random rand;

   constexpr auto NIters     = 1'000'000'000_u64;
   constexpr auto NTotalBits = NIters * 64_u64;
             auto nSetBits   = 0_u64;

   for (auto j = 0_u64; j < NIters; ++j)
   {
      nSetBits += std::bitset<64>(rand.gen<uint64>()).count();
   }

   return {
      {"NTotalBits", NTotalBits},
      {"NSetBits",   nSetBits}
   };
}

// /** run
//  *
//  * @brief Runs uniform bins test.
//  * 
//  * @note Amount of entries in each bin should be about equal.
//  *
//  * @return bool -- True if test passed, false otherwise.
//  */
// auto ym::ut::Random_UT::UniformBins_TC::run(void) -> std::unordered_map<std::string, double>
// {
//    ym::Random rand;

//    std::vector histo(1_u64 << 16_u64, 0_u64);

//    constexpr auto NIters = 1_u64 << 30_u64;

//    for (auto i = 0_u64; i < NIters; ++i)
//    {
//       auto const R = rand.gen<uint64>();

//       histo[(R >> 48_u64) & 0xffff_u64] += 1_u64;
//       histo[(R >> 32_u64) & 0xffff_u64] += 1_u64;
//       histo[(R >> 16_u64) & 0xffff_u64] += 1_u64;
//       histo[(R >>  0_u64) & 0xffff_u64] += 1_u64;
//    }

//    auto const NExpectedHitsPerBin = NIters / histo.size();

//    return std::unordered_map<std::string, float64>(); // TODO
// }

// /** run
//  *
//  * @brief Runs the bin frequency test.
//  * 
//  * @todo I want to use ym::MemMan, add dependency for that test to run first.
//  *
//  * @return bool -- True if test passed, false otherwise.
//  */
// auto ym::ut::Random_UT::BinFrequency_TC::run(void) -> std::unordered_map<std::string, double>
// {
//    ym::Random rand;

//    return std::unordered_map<std::string, double>();
// }
