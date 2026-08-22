/**
 * @file    testsuite.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#define YM_UNITTEST_ACTIVE_DEFINED
#include "ymdefs.h"

#include "testsuitebase.h"

namespace ym::unit::fileio
{

/**
 * @brief Test suite for FileIO.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UNIT_TESTCASE(InteractiveInspection)
   YM_UNIT_TESTCASE(SmokeTest)
};

} // ym::unit::fileio
