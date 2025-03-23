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
 * @brief Test suite for YmDefs.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UT_TESTCASE(InteractiveInspection)
   YM_UT_TESTCASE(BigFiveDeleteMacros  )
   YM_UT_TESTCASE(OverloadMacros       )
};

} // ym::ut
