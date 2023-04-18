/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"
#include "testsuite.h"

#include "timer.h"

/** TestSuite
*
* @brief Constructor.
*/
ym::ut::TestSuite::TestSuite(void)
   : TestSuiteBase("Timer")
{
   addTestCase<ElapsedTime>();
}

/** run
 *
 * @brief TODO.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::ElapsedTime::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}
