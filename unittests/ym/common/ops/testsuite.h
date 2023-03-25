/**
 * @file    testsuite.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ut.h"

#include "testsuitebase.h"

namespace ym::ut
{

/** TestSuite
 *
 * @brief Test suite for SUT Ops.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UT_TESTCASE(Casting)
};

} // ym::ut
