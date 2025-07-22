/**
 * @file    testsuite.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 * 
 * @note File used in unittests - maximum standard C++20.
 */

#pragma once

#define YM_FORCE_CPP_STANDARD_20_DEFINED
#include "ymdefs.h"
#undef YM_FORCE_CPP_STANDARD_20_DEFINED

#include "testsuitebase.h"

namespace ym::ut
{

/** TestSuite
 *
 * @brief Test suite for Logger.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UT_TESTCASE(InteractiveInspection)
};

} // ym::ut
