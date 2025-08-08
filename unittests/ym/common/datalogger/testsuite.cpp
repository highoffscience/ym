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

   DataLogger blackbox(10_u64);

   auto a = 0_i32;
   auto b = 0_f64;
   blackbox.addEntry("a", &a);
   blackbox.addEntry("b", &b);

   for (auto i = 0_u32; i < 15_u32; ++i)
   {
      blackbox.acquireAll();
      a++;
      b++;
   }

   auto const DataDumpSuccessful = blackbox.dump("ym/common/datalogger/data.csv");

   return {
      {"Success", DataDumpSuccessful}
   };
}
