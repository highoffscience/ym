/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"

#include "textlogger.h"
#include "ymglobals.h"

#include "datalogger.h" // Structures under test

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void) :
   TestSuiteBase("DataLogger")
{
   addTestCase<InteractiveInspection>();
}

/** run
 *
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_DataLogger);

   auto const SE2 = ymLogPushEnable(VG::Error); // TODO

   DataLogger blackbox(30uz, 10u);

   auto a = 1;
   auto b = 1.0;
   blackbox.track("a", &a);
   blackbox.track("b", &b);

   blackbox.ready();

   for (auto i = 0u; i < 100u; ++i)
   {
      blackbox.acquireAll();
      a++;
      b *= 1.5;
   }

   auto const DataDumpSuccessful = blackbox.dump("logs/data.csv");

   return {
      {"Success", DataDumpSuccessful}
   };
}
