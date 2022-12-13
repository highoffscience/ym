/**
 * @file    testcase.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ut.h"

#include "datashuttle.h"
#include "nameable.h"

#include <string>

namespace ym::ut
{

/** YM_UT_TESTCASE
 * 
 * @brief Defines a test case.
 */
#define YM_UT_TESTCASE(Name_)                                            \
   class Name_ : public TestCase                                         \
   {                                                                     \
   public:                                                               \
      explicit Name_(void)                                               \
         : TestCase(#Name_)                                              \
      { }                                                                \
      virtual ~Name_(void) = default;                                    \
                                                                         \
      virtual DataShuttle run(DataShuttle const & InData = {}) override; \
   };

/** TestCase
 * 
 * @brief Represents a test case.
 */
class TestCase : public Nameable
{
public:
   explicit TestCase(std::string name);
   virtual ~TestCase(void) = default;

   virtual DataShuttle run(DataShuttle const & InData = {}) = 0;
};

} // ym::ut
