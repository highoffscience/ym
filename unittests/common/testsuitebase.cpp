/**
 * @file    testsuitebase.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuitebase.h"

#include <algorithm>
#include <string>
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
 * @throws TSBError_TestCaseFailure  -- If there is an unhandled test case exception.
 * @throws TSBError_TestCaseNotFound -- If requested test case is not found.
 * 
 * @param Name   -- Name of test case to run.
 * @param InData -- Additional input data for test case.
 * 
 * @returns DataShuttle -- Results of test case.
 */
auto ym::ut::TestSuiteBase::runTestCase(std::string const & Name,
                                        DataShuttle const & InData) -> DataShuttle
{
   DataShuttle ds{};

   auto const It = std::find_if(_testCases.begin(), _testCases.end(),
      [Name](TCContainer_T::value_type const & Uptr) { return Uptr->getName() == Name; });

   if (It != _testCases.end())
   { // test case found
      try
      { // catch any uncaught/unaccounted for errors
         ds = (*It)->run(InData);
      }
      catch (std::exception const & E)
      { // re-throw a class cppyy knows how to convert - cppyy doesn't know about the ymassert_Base class
         throw TSBError_TestCaseFailure(E.what());
      }
   }
   else
   { // test case not found
      using namespace std::string_literals;
      throw TSBError_TestCaseNotFound("Test case"s + Name + " not found"s);
   }

   return ds;
}
