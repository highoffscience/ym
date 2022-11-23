/**
 * @file    random_ut.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "random_ut.h"

#include "random.h"

/** Random_UT
 *
 * @brief Constructor.
 */
ym::ut::Random_UT::Random_UT(void)
{
}

/** runTests
 *
 * @brief Runs all the unit tests.
 *
 * TODO
 */
void ym::ut::Random_UT::runTests(void)
{
   // Binomial
   int const B[16] = {
//   0000 0001 0010 0011 0100 0101 0110 0111
      -4,  -2,  -2,   0,  -2,   0,   0,  +2,

//   1000 1001 1010 1011 1100 1101 1110 1111
      -2,   0,   0,  +2,   0,  +2,  +2,  +4
   };

   ym::Random rand;

   constexpr unsigned long NIters = 1'000;
   unsigned long histo[16] = {0};

   for (auto i = 0ul; i < NIters; ++i)
   {
      auto const R = rand.gen<unsigned int>();

      auto const Bin = R & 0x00'00'00'0fu;
   }
}
