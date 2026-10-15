/**
 * @file    testsuitebase.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "datashuttle.h"
#include "testcase.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace ym::intg
{

/**
 * @brief Base class for unit test suites.
 */
class TestSuiteBase
{
public:
   using TestCaseArray_T = std::vector<std::unique_ptr<TestCase>>;

   explicit TestSuiteBase(std::string name) noexcept;
   virtual ~TestSuiteBase(void) = default;

   template <
      typename    DerivedTestCase_T,
      typename... Args_T>
   void addTestCase(Args_T &&... args);

   DataShuttle runTestCase(
      std::string const & Name,
      DataShuttle const & InData = {});

   inline auto const & getName(void) const noexcept { return _Name; }

private:
   std::string const _Name      {};
   TestCaseArray_T   _testCases {};
};

/**
 * @brief Adds test case to list of known test cases.
 *
 * @throws std::exception -- Whatever std::make_unique() throws.
 *
 * @tparam DerivedTestCase_T -- Test case to add.
 * @tparam Args_T            -- Type of additional arguments to test case.
 *
 * @param args -- Additional arguments to test case.
 */
template <
   typename    DerivedTestCase_T,
   typename... Args_T>
void TestSuiteBase::addTestCase(Args_T &&... args)
{
   static_assert(std::is_base_of_v<TestCase, DerivedTestCase_T>, "Can only add TestCase types");

   _testCases.emplace_back(
      std::make_unique<DerivedTestCase_T>(
         std::forward<Args_T>(args)...));
}

} // ym::intg
