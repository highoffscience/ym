/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ym.h"
#include "testsuite.h"

#include "memio.h"

#include "textlogger.h"
#include "ymception.h"

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void)
   : TestSuiteBase("MemIO")
{
   addTestCase<name_of_test_case_here>();
}

/** run
 *
 * @brief TODO.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::name_of_test_case_here::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SC = ymLogPushEnable(VG::UnitTest_MemIO);

   return {};
}
