/**
 * @file    unittestbase.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ut.h"

#include "datashuttle.h"
#include "testcase.h"
#include "utception.h"

#include <exception>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ut
{

/** UnitTestBase
 *
 * @brief Base class for unit test suites.
 */
class UnitTestBase
{
public:
   using TCContainer_T      = std::vector<std::unique_ptr<TestCase>>;
   using SuiteDataShuttle_T = std::unordered_map<std::string, DataShuttle>;

   explicit UnitTestBase(std::string && name_uref);
   virtual ~UnitTestBase(void) = default;

   auto const & getName     (void) const { return _Name;      }
   auto const & getTestCases(void) const { return _testCases; }

   template <typename    DerivedTestCase_T,
             typename... Args_T>
   void addTestCase(Args_T &&... args_uref);

   SuiteDataShuttle_T runAllTestCases(void);

   DataShuttle runTestCase(std::string const & Name,
                           DataShuttle const & InData = {});

   UT_DEFN_UTCEPTION(TestCaseNotFoundException)

private:
   std::string   const _Name;
   TCContainer_T       _testCases;
};

/** addTestCase
 *
 * @brief Adds test case to list of known test cases.
 * 
 * @tparam DerivedTestCase_T -- Test case to add.
 * @tparam Args_T            -- Type of additional arguments to test case.
 * 
 * @param args_uref -- Additional arguments to test case.
 */
template <typename    DerivedTestCase_T,
          typename... Args_T>
void UnitTestBase::addTestCase(Args_T &&... args_uref)
{
   static_assert(std::is_base_of_v<TestCase, DerivedTestCase_T>, "Can only add TestCase types");

   _testCases.emplace_back(new DerivedTestCase_T(std::forward<Args_T>(args_uref)...));
}

} // ut
