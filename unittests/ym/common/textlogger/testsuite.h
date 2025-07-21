/**
 * @file    testsuite.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#define YM_FORCE_CPP_STANDARD_20_DEFINED
#include "ymdefs.h"
#undef YM_FORCE_CPP_STANDARD_20_DEFINED

#include "testsuitebase.h"

namespace ym::ut
{

/** TestSuite
 *
 * @brief Test suite for SUT TextLogger.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UT_TESTCASE(InteractiveInspection)
   YM_UT_TESTCASE(OpenAndClose         )
};

} // ym::ut
