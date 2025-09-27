/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"

#include "textlogger.h"
#include "ymglobals.h"

#include "fileio.h" // Structures under test

/** TestSuite
 *
 * @brief Constructor.
 */
ym::unit::TestSuite::TestSuite(void) :
   TestSuiteBase("FileIO")
{
   addTestCase<InteractiveInspection>();
}

/** run
 *
 * @brief TODO.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_FileIO);

   auto firstChar = '!'; // '!' not in char set for file
   auto buffer = FileIO::createFileBuffer("ym/common/fileio/data.txt");
   if (buffer) // TODO buffer.or_else(...)? to initialize first char
   {
      firstChar = (*buffer)[0];
   }

   return {
      {"E0", firstChar}
   };
}
