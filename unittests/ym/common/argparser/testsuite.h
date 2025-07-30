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

namespace ym::ut
{

/** TestSuite
 *
 * @brief Test suite for ArgParser.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_UT_TESTCASE(BasicParse   )
   YM_UT_TESTCASE(FlagIntegrity)
   YM_UT_TESTCASE(SizeOfArg    )
};

} // ym::ut
