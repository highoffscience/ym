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

   DataLogger blackbox(10u);

   auto a = 0;
   // auto b = 0.0;
   blackbox.addEntry("a", &a);
   // blackbox.addEntry("b", &b);

   for (auto i = 0u; i < 15u; ++i)
   {
      // blackbox.acquireAll();
      a++;
      // b++;
   }

   // auto const DataDumpSuccessful = blackbox.dump("ym/common/datalogger/data.csv");

   return {
      {"Success", true /*DataDumpSuccessful*/}
   };
}
