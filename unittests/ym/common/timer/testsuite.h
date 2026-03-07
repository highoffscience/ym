/**
 * @file    testsuite.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 *
 * @note File used in unittests - maximum standard C++20.
 */

#pragma once

#define YM_UNITTEST_ACTIVE_DEFINED
#include "ymdefs.h"

#include "testsuitebase.h"

namespace ym::unit
{

/** TestSuite
 *
 * @brief Test suite for Timer.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UNIT_TESTCASE(InteractiveInspection)
   YM_UNIT_TESTCASE(SmokeTest)
   YM_UNIT_TESTCASE(VerifyTimer)
};

} // ym::unit
