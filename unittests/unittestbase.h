/**
 * @file    unittestbase.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym_ut.h"


#include "testcase.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ym::ut
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

   explicit UnitTestBase(std::string   && name_uref,
                         TCContainer_T && tcs_uref);
   virtual ~UnitTestBase(void) = default;

   auto const & getName     (void) const { return _Name;      }
   auto const & getTestCases(void) const { return _testCases; }

   SuiteDataShuttle_T runAllTestCases(void);

   DataShuttle runTestCase(std::string const & Name);
   DataShuttle runTestCase(std::string const & Name,
                           DataShuttle const & InData);

private:
   std::string   const _Name;
   TCContainer_T       _testCases;
};

} // ym::ut
