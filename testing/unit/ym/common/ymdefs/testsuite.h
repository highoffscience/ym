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

namespace ym::unit::ymdefs
{

/**
 * @brief Test suite for YmDefs.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UNIT_TESTCASE(InteractiveInspection)
   YM_UNIT_TESTCASE(SmokeTest)
   YM_UNIT_TESTCASE(BigFiveDeleteMacros)
   YM_UNIT_TESTCASE(OverloadMacros)
   YM_UNIT_TESTCASE(Func_getNBits)
   YM_UNIT_TESTCASE(Func_empty)
   YM_UNIT_TESTCASE(PrimitiveDefSuffixes)
};

} // ym::unit::ymdefs
