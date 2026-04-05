/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"

#include "globallogger.h"
#include "ymglobals.h"

#include "datalogger.h" // Structures under test

/** TestSuite
 *
 * @brief Constructor.
 */
ym::unit::TestSuite::TestSuite(void) :
   TestSuiteBase("DataLogger")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
}

/** run
 *
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);
   return {{}};
}

/** run
 *
 * @brief Basic integrity test.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::SmokeTest::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);

   DataLogger blackbox(10uz, 10uz);

   auto a = 0;
   auto b = 1.0;
   auto c = false;
   auto d = 2.0;
   blackbox.track("a", &a);
   blackbox.track("b", &b);
   blackbox.track("c", &c);
   blackbox.track("d", &d);

   for (auto i = 0u; i < 100u + 1u; ++i)
   {
      blackbox.acquire();
      a++;
      b *= 1.1;
      c = !c;
      d *= 1.1;
   }

   // DataLogger::Options_T options;
   // // options._dumpMode = DataLogger::DumpMode_T::Binary;
   // options._openingOptions._filenameMode  = Logger::FilenameMode_T::KeepOriginal;
   // options._openingOptions._overwriteMode = Logger::OverwriteMode_T::Allow;
   auto const DataDumpSuccessful = blackbox.dump("logs/data.csv");

   return {
      {"Success", DataDumpSuccessful}
   };
}
