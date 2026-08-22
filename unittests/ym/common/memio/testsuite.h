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

namespace ym::unit::memio
{

/** TestSuite
 *
 * @brief Test suite for MemIO.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default; // TODO we don't need this explicitly defaulted - it is virtual by default

   YM_UNIT_TESTCASE(InteractiveInspection)
   YM_UNIT_TESTCASE(SmokeTest)
   YM_UNIT_TESTCASE(StackAlloc)
   YM_UNIT_TESTCASE(StackString)
};

} // ym::unit::memio
