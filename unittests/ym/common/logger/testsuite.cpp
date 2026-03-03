/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"

#include "globallogger.h"
#include "ymglobals.h"

#include "logger.h" // Structures under test

/** TestSuite
 *
 * @brief Constructor.
 */
ym::unit::TestSuite::TestSuite(void) :
   TestSuiteBase("Logger")
{
   addTestCase<InteractiveInspection>();
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
   ymLog(VF::UnitTest, "Go! Torchic!");
   // GlobalLogger::getGlobalInstance()->close(); // TODO logger waits on close, which could happen in this
   // destructor, but this class is created in the python script, and the python script won't exit until
   // the logger closes. The logger needs a close command somewhere so the script can continue...
   // It also appears the logger is not using the expected name, it is using the default name...
   // The python script can't create the global logger because the global logger uses c++23 standard, where
   // cppyy can only use c++20.
   return {{"", true}};
}
