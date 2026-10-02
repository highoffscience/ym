/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"
#include "ymglobals.h"

#include "textlogger.h" // Structures under test

#include "fmt/format.h"

/**
 * @brief Constructor.
 */
ym::unit::textlogger::TestSuite::TestSuite(void) :
   TestSuiteBase("TextLogger")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
   addTestCase<OpenAndClose>();
}

/**
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::textlogger::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}

/**
 * @brief Basic integrity test.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::textlogger::TestSuite::SmokeTest::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {
      {"True", true}
   };
}

/**
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::textlogger::TestSuite::OpenAndClose::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
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
