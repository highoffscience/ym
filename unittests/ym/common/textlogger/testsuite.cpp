/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"

#include "textlogger.h"
#include "ymglobals.h"

#include "textlogger.h" // Structures under test

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void) :
   TestSuiteBase("TextLogger")
{
   addTestCase<InteractiveInspection>();
   addTestCase<OpenAndClose         >();
}

/** run
 *
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_TextLogger);
   return {};
}

/** run
 *
 * @brief Opens and closes the text logger.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::OpenAndClose::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_TextLogger);

   TextLogger t("ym/common/textlogger/log.txt");
   auto const IsOpen = t.open();
   t.enable(VG::UnitTest_TextLogger);
   t.printf(VG::UnitTest_TextLogger, "Go! Torchic!");

   ymLog(VG::UnitTest_TextLogger, "Go! Pumpkaboo!");

   t.close(); // writer thread is joined with the thread that calls this
   auto const IsClosed = !t.isOpen();

   return {
      {"IsOpen",   IsOpen},
      {"IsClosed", IsClosed}
   };
}
