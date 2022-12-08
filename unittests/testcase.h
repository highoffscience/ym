/**
 * @file    testcase.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym_ut.h"

#include "datashuttle.h"

#include <string>

namespace ym::ut
{

/** YM_UT_TESTCASE
 * 
 * @brief Defines a test case.
 */
#define YM_UT_TESTCASE(Name_)                                            \
   class Name_##_TC : public TestCase                                    \
   {                                                                     \
   public:                                                               \
      explicit Name_##_TC(void)                                          \
         : TestCase(#Name_)                                              \
      { }                                                                \
      virtual ~Name_##_TC(void) = default;                               \
                                                                         \
      virtual DataShuttle run(DataShuttle const & InData = {}) override; \
   };

/** TestCase
 * 
 * @brief Represents a test case.
 */
class TestCase
{
public:
   explicit TestCase(std::string && name_uref);
   virtual ~TestCase(void) = default;

   virtual DataShuttle run(DataShuttle const & InData = {}) = 0;

   auto const & getName(void) const { return _Name; }

private:
   std::string const _Name;
};

} // ym::ut
