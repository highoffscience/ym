/**
 * @file    unittestbase.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym_ut.h"

#include <string>
#include <vector>

namespace ym::ut
{

/** YM_UT_TESTCASE
 * 
 * @brief Defines a test case.
 */
#define YM_UT_TESTCASE(Name_)          \
   class Name_##_TC : public TestCase  \
   {                                   \
   public:                             \
      explicit Name_##_TC(void)        \
         : TestCase(#Name_)            \
      { }                              \
                                       \
      virtual bool run(void) override; \
   };

/** UnitTestBase
 *
 * @brief Base class for unit test suites.
 */
class UnitTestBase
{
public:
   virtual ~UnitTestBase(void) = default;

   auto const & getName      (void) const { return _Name;       }
   auto const & getDependents(void) const { return _Dependents; }

protected:
   explicit UnitTestBase(std::string              && _name_uref,
                         std::vector<std::string> && _dependents_uref);

   /** Test
    * 
    * @brief Represents a test case.
    */
   class TestCase
   {
   public:
      virtual bool run(void) = 0;

   protected:
      explicit TestCase(std::string && _name_uref);

   private:
      std::string const _Name;
   };

private:
   std::string              const _Name;
   std::vector<std::string> const _Dependents;
};

} // ym::ut
