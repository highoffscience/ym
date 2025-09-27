/**
 * @file    testsuite.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 * 
 * @note File used in unittests - maximum standard C++20.
 */

#pragma once

#include "testsuitebase.h"
#include "ymdefs.h"

namespace ym::unit
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

} // ym::unit
