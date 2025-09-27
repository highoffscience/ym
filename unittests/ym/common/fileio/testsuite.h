/**
 * @file    testsuite.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "testsuitebase.h"

namespace ym::unit // TODO consider renaming namespace unit, YM_UT_TESTCASE -> YM_UNIT_TESTCASE
{

/** TestSuite
 *
 * @brief Test suite for FileIO.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UT_TESTCASE(InteractiveInspection)
};

} // ym::unit
