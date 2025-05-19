/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"
#include "testsuite.h"

#include "ymassert.h"

#include "textlogger.h"
#include "ymassert.h"

#include <cstring>

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void)
   : TestSuiteBase("YmAssert")
{
   addTestCase<InteractiveInspection>();
   addTestCase<What                 >();
}

/** run
 *
 * @brief TODO.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   // TODO rename to UnitTest_YmAssert
   auto const SE = ymLogPushEnable(VG::UnitTest_YmError);

   YM_DECL_YMASSERT(Error)

   auto const I = 9;
   auto const J = 5;

   YMASSERT(I > J, Error, YM_DAH,
      "I ({}) is NOT greater than J ({})", I, J);

   return {
      {"True", false}
   };
}

// namespace ym
// {
//    struct Test
//    {
//       YM_DECL_YMERROR(Ym_UT_YmError_What_Error)
//    };
// } // ym

/** run
 *
 * @brief TODO.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::What::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmError);

   bool generatedAppropriateMsg = false;

   try
   {
      // Test::Ym_UT_YmError_What_Error::check(false, "Go! Torchic!");
   }
   catch (std::exception const & E)
   {
      // TODO
      // str const ExpectedMsg =
      //    "Ym_UT_YmError_What_Error \"/home/forrest/code/ym/unittests/ym/common/ymerror/testsuite.cpp:65\": Go! Torchic!";
      // ymLog(VG::UnitTest_YmError, "--> %s", E.what());
      // generatedAppropriateMsg = std::strcmp(E.what(), ExpectedMsg) == 0;
   }

   return {
      {"GAM", generatedAppropriateMsg}
   };
}
