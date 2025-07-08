/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"

#include "textlogger.h"
#include "ymglobals.h"

#include "ymassert.h" // Structures under test

#include <cstring>
#include <string>

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void) :
   TestSuiteBase("YmAssert")
{
   addTestCase<InteractiveInspection>();
   addTestCase<What                 >();
}

/** run
 *
 * @brief Sandbox.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmAssert);

   YM_DECL_YMASSERT(Error)

   auto const I = 9;
   auto const J = 5;

   auto errored = false; // until told otherwise

   try
   {
      YMASSERT(I < J, Error, YM_DAH, "I ({}) is NOT less than J ({})", I, J);
   }
   catch (ymassert_Base const & E)
   {
      errored = true;
   }

   return {
      {"Errored", errored}
   };
}

/** run
 *
 * @brief TODO.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::What::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmAssert);

   bool expectedMsg = false;

   YM_DECL_YMASSERT(Error)

   try
   {
      YMASSERT(false, Error, YM_DAH, "Go! {}!", "Torchic");
   }
   catch (std::exception const & E)
   {
      str const ExpectedMsg =
         "Assert @ \"/home/forrest/code/ym/unittests/ym/common/ymassert/testsuite.cpp:75\": Go! Torchic!";
      expectedMsg = std::strcmp(E.what(), ExpectedMsg) == 0;
   }

   return {
      {"ExpectedMsg", expectedMsg}
   };
}
