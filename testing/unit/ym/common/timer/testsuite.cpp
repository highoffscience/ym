/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"
#include "ymglobals.h"

#include "timer.h" // Structures under test

#include "fmt/format.h"

/**
 * @brief Constructor.
 */
ym::unit::timer::TestSuite::TestSuite(void) :
   TestSuiteBase("Timer")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
   addTestCase<VerifyTimer>();
}

/**
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::timer::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}

/**
 * @brief Basic integrity test.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::timer::TestSuite::SmokeTest::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {
      {"True", true}
   };
}

/**
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::timer::TestSuite::VerifyTimer::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {
      {"True", true}
   };
}
