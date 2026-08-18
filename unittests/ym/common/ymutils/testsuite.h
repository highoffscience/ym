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

namespace ym::unit::ymutils
{

/**
 * @brief Test suite for YmUtils.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UNIT_TESTCASE(InteractiveInspection)
   YM_UNIT_TESTCASE(SmokeTest)
   YM_UNIT_TESTCASE(Func_castPtrTo)

   YM_UNIT_TESTCASE(Funcs)
   YM_UNIT_TESTCASE(PtrIntClass)
   YM_UNIT_TESTCASE(ByteBitsetClass)
   YM_UNIT_TESTCASE(BoundedPtrClass)
   YM_UNIT_TESTCASE(PolyRawClass)
};

} // ym::unit
