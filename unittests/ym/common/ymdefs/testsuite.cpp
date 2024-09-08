/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"
#include "testsuite.h"

#include "textlogger.h"
#include "ymerror.h"

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void)
   : TestSuiteBase("YmDefs")
{
   addTestCase<InteractiveInspection>();
   addTestCase<BigFiveDeleteMacros  >();
}

/** run
 *
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmDefs);

   return {};
}

/** run
 *
 * @brief Tests the "big five" delete macros.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::BigFiveDeleteMacros::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmDefs);

   struct Test
   {
      YM_NO_DEFAULT    (Test)
      YM_NO_COPY       (Test)
      YM_NO_ASSIGN     (Test)
      YM_NO_MOVE_COPY  (Test)
      YM_NO_MOVE_ASSIGN(Test)
   };

   return {
      {"Defined", true}
   };
}
