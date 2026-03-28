/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"

#include "globallogger.h"
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
   addTestCase<SmokeTest>();
}

/** run
 *
 * @brief TODO.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);

   // auto firstChar = '!'; // '!' not in char set for file
   // auto buffer = FileIO::createFileBuffer("ym/common/fileio/data.txt");
   // if (buffer) // TODO buffer.or_else(...)? to initialize first char
   // {
   //    firstChar = (*buffer)[0];
   // }

   // return {
   //    {"E0", firstChar}
   // };

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

   strlit const Filename = "ym/common/fileio/data.txt";
   auto const Exists = FileIO::exists(Filename);
   auto const NotExists = FileIO::exists("ym/common/fileio/no_exists.txt");

   FileIO f(Filename);

   char buffer[1024]{};
   auto const Created = f.createFileBuffer(buffer);

   return {{}};
}
