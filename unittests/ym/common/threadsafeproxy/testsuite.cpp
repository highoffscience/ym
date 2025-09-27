/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"
#include "testsuite.h"

#include "threadsafeproxy.h"

#include "textlogger.h"
#include "ymerror.h"

/** TestSuite
 *
 * @brief Constructor.
 */
ym::unit::TestSuite::TestSuite(void)
   : TestSuiteBase("ThreadSafeProxy")
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
   auto const SE = ymLogPushEnable(VG::UnitTest_ThreadSafeProxy);

   struct A { int _i; };

   A a{._i = 9};

   ThreadSafeProxy tsp(&a);
   tsp->_i = 7;

   return {
      {"i", a._i}
   };
}
