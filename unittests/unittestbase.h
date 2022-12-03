/**
 * @file    unittestbase.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym_ut.h"

#include "testcase.h"

#include <any>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

namespace ym::ut
{

/** UnitTestBase
 *
 * @brief Base class for unit test suites.
 */
class UnitTestBase
{
public:
   virtual ~UnitTestBase(void) = default;

   using TCContainer_T      = std::set<std::unique_ptr<TestCase>, /*TODO add comparison op - i think*/>;
   using TCDataShuttle_T    = TestCase::DataShuttle_T;
   using SuiteDataShuttle_T = std::unordered_map<std::string, TCDataShuttle_T>;

   auto const & getName     (void) const { return _Name;      }
   auto const & getTestCases(void) const { return _testCases; }

   SuiteDataShuttle_T runAllTestCases(void);

   TCDataShuttle_T runTestCase(std::string const & Name);

protected:
   explicit UnitTestBase(std::string   && _name_uref,
                         TCContainer_T && _tcs_uref);

private:
   std::string   const _Name;
   TCContainer_T       _testCases;
};

} // ym::ut
