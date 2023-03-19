/**
 * @file    testsuitebase.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuitebase.h"

#include "fmt/core.h"

#include <algorithm>
#include <utility>

/** TestSuiteBase
 *
 * @brief Constructor.
 */
ym::ut::TestSuiteBase::TestSuiteBase(std::string name)
   : Nameable(std::move(name)),
     _testCases {/*default*/}
{
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
auto ym::ut::TestSuiteBase::runTestCase(std::string const & Name,
                                        DataShuttle const & InData) -> DataShuttle
{
   DataShuttle ds{};

   auto const It = std::find_if(_testCases.begin(), _testCases.end(),
      [Name](TCContainer_T::value_type const & Uptr) { return Uptr->getName() == Name; });

   if (It != _testCases.end())
   { // test case found
      ds = (*It)->run(InData);
   }
   else
   { // test case not found
      throw TestCaseNotFoundException(fmt::format("Test case {} not found", Name));
   }

   return ds;
}
