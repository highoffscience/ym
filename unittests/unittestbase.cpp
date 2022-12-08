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
ym::ut::UnitTestBase::UnitTestBase(std::string   && name_uref,
                                   TCContainer_T && tcs_uref)
   : _Name      {std::move(name_uref)},
     _testCases {std::move(tcs_uref )}
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
      outData[tc_ref->getName()] = tc_ref->run();
   }

   return outData;
}

/** runTestCase
 *
 * @brief TODO.
 */
auto ym::ut::UnitTestBase::runTestCase(std::string const & Name) -> DataShuttle
{
   return runTestCase(Name, {});
}

/** runTestCase
 *
 * @brief TODO.
 */
auto ym::ut::UnitTestBase::runTestCase(std::string const & Name,
                                       DataShuttle const & InData) -> DataShuttle
{
   DataShuttle ds{};

   auto const It = std::find_if(_testCases.begin(), _testCases.end(),
      [Name](TCContainer_T::value_type const & Uptr) { return Uptr->getName() == Name; });

   if (It != _testCases.end())
   { // found test case
      ds = (*It)->run(InData);
   }

   return ds;
}
