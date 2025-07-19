/**
 * @file    testsuitebase.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#define YM_FORCE_CPP_STANDARD_20_DEFINED
#include "nameable.h"
#include "ymdefs.h"
#undef YM_FORCE_CPP_STANDARD_20_DEFINED

#include "datashuttle.h"
#include "testcase.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace ym::ut
{

/** TestSuiteBase
 *
 * @brief Base class for unit test suites.
 */
class TestSuiteBase : public PermaNameable_NV<>
{
public:
   using TCArray_T = std::vector<std::unique_ptr<TestCase>>;

   explicit TestSuiteBase(std::string name);
   virtual ~TestSuiteBase(void) = default;

   template <
      typename    DerivedTestCase_T,
      typename... Args_T>
   void addTestCase(Args_T &&... args_uref);

   DataShuttle runTestCase(
      std::string const & Name,
      DataShuttle const & InData = {});

private:
   TCArray_T _testCases{};
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
template <
   typename    DerivedTestCase_T,
   typename... Args_T>
void TestSuiteBase::addTestCase(Args_T &&... args_uref)
{
   static_assert(std::is_base_of_v<TestCase, DerivedTestCase_T>, "Can only add TestCase types");

   _testCases.emplace_back(new DerivedTestCase_T(std::forward<Args_T>(args_uref)...));
}

} // ym::ut
