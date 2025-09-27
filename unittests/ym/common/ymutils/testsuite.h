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
 * @brief Test suite for YmUtils.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UT_TESTCASE(InteractiveInspection)
   YM_UT_TESTCASE(PtrToIntConversion   )
   YM_UT_TESTCASE(BoundedPtrClass      )
   YM_UT_TESTCASE(BinarySearch         )
   YM_UT_TESTCASE(BoundedStr           )
   YM_UT_TESTCASE(PtrCast              )
   YM_UT_TESTCASE(BitSet               )
   YM_UT_TESTCASE(PolyRawTest          )
};

} // ym::unit
