/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"

#include "textlogger.h"
#include "ymglobals.h"

#include "ymdefs.h" // Structures under test

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void) :
   TestSuiteBase("YmDefs")
{
   addTestCase<InteractiveInspection>();
   addTestCase<BigFiveDeleteMacros  >();
   addTestCase<OverloadMacros       >();
}

/** run
 *
 * @brief Sandbox.
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

/** run
 *
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::OverloadMacros::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmDefs);

   #define YM_SUM(...) YM_MACRO_OVERLOAD(YM_SUM, __VA_ARGS__)
   #define YM_SUM1(First) (First)
   #define YM_SUM2(First, Second) (YM_SUM1(First) + YM_SUM1(Second))
   #define YM_SUM3(First, Second, Third) (YM_SUM2(First, Second) + YM_SUM1(Third))
   #define YM_SUM4(First, Second, Third, Fourth) (YM_SUM3(First, Second, Third) + YM_SUM1(Fourth))

   auto const Sum = YM_SUM(1, 2, 3, 4);

   return {
      {"Sum", Sum}
   };
}
