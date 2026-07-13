/**
 * @file    testsuite.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 *
 * @note File used in unittests - maximum standard C++20.
 */

#pragma once

#include "ymdefs.h"

#include "testsuitebase.h"

namespace ym::unit
{

/** TestSuite
 *
 * @brief Test suite for MemIO.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UNIT_TESTCASE(InteractiveInspection)
   YM_UNIT_TESTCASE(SmokeTest)
   YM_UNIT_TESTCASE(StackAlloc)
};

} // ym::unit
