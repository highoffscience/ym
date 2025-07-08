/**
 * @file    testsuite.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "testsuitebase.h"
#include "ymdefs.h"

namespace ym::ut
{

/** TestSuite
 *
 * @brief Test suite for YmAssert.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UT_TESTCASE(InteractiveInspection)
   YM_UT_TESTCASE(What                 )
};

} // ym::ut
