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
   : UnitTestBase("Random", {"MemoryManager"})
{
}

/** runTest_BinFrequency
 *
 * @brief Runs the bin frequency test.
 * 
 * @todo I want to use ym::MemMan, add dependency for that test to run first.
 *
 * @return bool -- True if test passed, false otherwise.
 */
bool ym::ut::Random_UT::runTest_BinFrequency(void)
{
   ym::Random rand;

   return false;
}
