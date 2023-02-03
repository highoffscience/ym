/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ym.h"
#include "testsuite.h"

#include "textlogger.h"

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void)
   : TestSuiteBase("TextLogger")
{
   addTestCase<OpenAndClose>();
}

/** run
 *
 * @brief TODO.
 *
 * @return DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::OpenAndClose::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   TextLogger t;
   t.openToStdout();
   t.enable(VGM_T::Logger);
   t.printf(VGM_T::Logger, "Go! Torchic!");

   return {
      {"IsOpen", t.isOpen()}
   };
}
