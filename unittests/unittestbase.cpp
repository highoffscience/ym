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
ut::UnitTestBase::UnitTestBase(std::string && name_uref)
   : _Name      {std::move(name_uref)},
     _testCases {/*default*/         }
{
}

/** runAllTestCases
 *
 * @brief Runs all registered test cases.
 * 
 * @return SuiteDataShuttle_T -- Results of all run test cases.
 */
auto ut::UnitTestBase::runAllTestCases(void) -> SuiteDataShuttle_T
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
 * @brief Runs specified test case.
 * 
 * @param Name   -- Name of test case to run.
 * @param InData -- Additional input data for test case.
 * 
 * @return DataShuttle -- Results of test case.
 */
TODO // throw exception if test case not found
auto ut::UnitTestBase::runTestCase(std::string const & Name,
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
