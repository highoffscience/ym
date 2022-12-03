/**
 * @file    unittestbase.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "unittestbase.h"

#include <algorithm>
#include <utility>

/** UnitTestBase
 *
 * @brief Constructor.
 */
ym::ut::UnitTestBase::UnitTestBase(std::string   && _name_uref,
                                   TCContainer_T && _tcs_uref)
   : _Name      {std::move(_name_uref)},
     _testCases {std::move(_tcs_uref )}
{
}

/** runAllTestCases
 *
 * @brief TODO.
 */
auto ym::ut::UnitTestBase::runAllTestCases(void) -> SuiteDataShuttle_T
{
   SuiteDataShuttle_T outData;

   for (auto & tc_ref : _testCases)
   {
      outData[tc_ref->getName()] =  tc_ref->run();
   }

   return outData;
}

/** runTestCase
 *
 * @brief TODO.
 */
auto ym::ut::UnitTestBase::runTestCase(std::string const & Name) -> TCDataShuttle_T
{
   return runTestCase(Name, {});
}

/** runTestCase
 *
 * @brief TODO.
 */
auto ym::ut::UnitTestBase::runTestCase(std::string     const & Name,
                                       TCDataShuttle_T const & InData) -> TCDataShuttle_T
{
   TCDataShuttle_T ds;

   auto const It = std::find_if(_testCases.begin(), _testCases.end(),
      [Name](TCContainer_T::value_type const & TUptr) { return TUptr->getName() == Name; });

   if (It != _testCases.end())
   { // found test case
      ds = (*It)->run(InData);
   }

   return ds;
}
