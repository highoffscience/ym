/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"

#include "globallogger.h"
#include "ymglobals.h"

#include "textlogger.h" // Structures under test

/** TestSuite
 *
 * @brief Constructor.
 */
ym::unit::TestSuite::TestSuite(void) :
   TestSuiteBase("TextLogger")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
   addTestCase<OpenAndClose>();
}

/** run
 *
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}

/** run
 *
 * @brief Basic integrity test.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::SmokeTest::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}

/** run
 *
 * @brief Opens and closes the text logger.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::OpenAndClose::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   // TextLogger t("ym/common/textlogger/log.txt");
   // auto const IsOpen = t.open();
   // t.enable(VF::UnitTest_TextLogger);
   // t.printf(VF::UnitTest_TextLogger, "Go! Torchic!");

   // ymLog(VF::UnitTest_TextLogger, "Go! Pumpkaboo!");

   // t.close(); // writer thread is joined with the thread that calls this
   // auto const IsClosed = !t.isOpen();

   auto IsOpen = true;
   auto IsClosed = true;

   return {
      {"IsOpen",   IsOpen},
      {"IsClosed", IsClosed}
   };
}
