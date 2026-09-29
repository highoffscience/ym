/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"
#include "ymglobals.h"

#include "logger.h" // Structures under test

#include "fmt/format.h"

/**
 * @brief Constructor.
 */
ym::unit::logger::TestSuite::TestSuite(void) :
   TestSuiteBase("Logger")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
}

/**
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::logger::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}

/**
 * @brief Basic integrity test.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::logger::TestSuite::SmokeTest::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   fmt::println("Go! Torchic!");
   return {
      {"IsOpen", GlobalLogger::getGlobalInstance()->isOpen()}
   };
}
