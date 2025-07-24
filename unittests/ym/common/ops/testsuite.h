/**
 * @file    testsuite.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "utdefs.h"

#include "testsuitebase.h"

namespace ym::ut
{

/** TestSuite
 *
 * @brief Test suite for Ops.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UT_TESTCASE(Casting   )
   YM_UT_TESTCASE(BadCasting)
};

} // ym::ut
