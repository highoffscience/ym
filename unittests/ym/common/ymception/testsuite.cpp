/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"
#include "testsuite.h"

#include "ymception.h"

#include "textlogger.h"
#include "ymception.h"

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void)
   : TestSuiteBase("Ymception")
{
   addTestCase<InteractiveInspection>();
}

/** run
 *
 * @brief TODO.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_Ymception);

   return {};
}
