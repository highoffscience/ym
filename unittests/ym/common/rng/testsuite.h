/**
 * @file    testsuite.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "utdefs.h"

#include "testsuitebase.h"

namespace ym::unit
{

/** TestSuite
 *
 * @brief Test suite for Random.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UT_TESTCASE(ZerosAndOnes);
   YM_UT_TESTCASE(UniformBins );
};

} // ym::unit
