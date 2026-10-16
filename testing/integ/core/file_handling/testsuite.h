/**
 * @file    testsuite.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#define YM_INTEG_TESTING_ACTIVE_DEFINED
#include "ymdefs.h"

#include "testsuitebase.h"

namespace ym::integ::file_handling
{

/**
 * @brief Test suite for FileIO.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_INTEG_TESTCASE(InteractiveInspection)
   YM_INTEG_TESTCASE(SmokeTest)
   YM_INTEG_TESTCASE(Class_FileIO)
};

} // ym::integ::file_handling
