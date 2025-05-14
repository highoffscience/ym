/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"
#include "testsuite.h"

#include "textlogger.h"

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void)
   : TestSuiteBase("TextLogger")
{
   addTestCase<OpenAndClose>();
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

   // TODO
   // TextLogger t;
   // // t.openToStdout();
   // t.enable(VG::TextLogger);
   // t.printf(VG::TextLogger, "Go! Torchic!");

   // auto const IsOpen = t.isOpen();

   // t.close(); // writer thread is joined with the thread that calls this
   // auto const IsClosed = !t.isOpen();

   // return {
   //    {"IsOpen",   IsOpen},
   //    {"IsClosed", IsClosed}
   // };

   return {};
}
