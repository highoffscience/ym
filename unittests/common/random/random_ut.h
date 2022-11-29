/**
 * @file    random_ut.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym_ut.h"

#include "unittestbase.h"

namespace ym::ut
{

/** Random_UT
 *
 * @brief Test suite for structure Random.
 */
class Random_UT : public UnitTestBase
{
public:
   explicit Random_UT(void);
   virtual ~Random_UT(void) = default;

   YM_UT_TESTCASE(ZerosAndOnes);
   // YM_UT_TESTCASE(UniformBins);
   // YM_UT_TESTCASE(BinFrequency);
};

} // ym::ut
