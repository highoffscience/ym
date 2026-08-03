/**
 * @file    testsuitebase.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 *
 * @note File used in unittests - maximum standard C++20.
 */

#pragma once

#include "nameable.h"
#include "ymdefs.h"

#include "datashuttle.h"
#include "testcase.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace ym::unit
{

/** TestSuiteBase
 *
 * @brief Base class for unit test suites.
 */
class TestSuiteBase : public PermaNameable_NV<>
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

private:
   TestCaseArray_T _testCases{};
};

/** addTestCase
 *
 * @brief Adds test case to list of known test cases.
 *
 * @throws Whatever std::make_unique() throws.
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

} // ym::unit
