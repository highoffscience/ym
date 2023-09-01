/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"
#include "testsuite.h"

#include "fileio.h"

#include "textlogger.h"
#include "ymception.h"

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void)
   : TestSuiteBase("FileIO")
{
   addTestCase<InteractiveInspection>();
}

/** run
 *
 * @brief TODO.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_FileIO);
   auto const SE2 = ymLogPushEnable(VG::FileIO);

   auto buffer = FileIO::createFileBuffer("ym/common/fileio/data.txt");

   return {
      {"E0", buffer[0]}
   };
}
