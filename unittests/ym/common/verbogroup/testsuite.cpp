/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"
#include "ymglobals.h"

#include "verbogroup.h" // Structures under test

#include "fmt/format.h"

/**
 * @brief Constructor.
 */
ym::unit::verbogroup::TestSuite::TestSuite(void) :
   TestSuiteBase("VerboGroup")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
}

/**
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::verbogroup::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}

/**
 * @brief Basic integrity test.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::verbogroup::TestSuite::SmokeTest::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {
      {"True", true}
   };
}
