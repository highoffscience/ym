/**
 * @file    testcase.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#define YM_FORCE_CPP_STANDARD_20_DEFINED
#include "nameable.h"
#include "ymdefs.h"
#undef YM_FORCE_CPP_STANDARD_20_DEFINED

#include "datashuttle.h"

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
      explicit Name_(void) : TestCase(#Name_) {}                         \
      virtual DataShuttle run(DataShuttle const & InData = {}) override; \
   };

/** TestCase
 * 
 * @brief Represents a test case.
 */
class TestCase : public PermaNameable_NV<>
{
public:
   explicit inline TestCase(std::string name) :
      PermaNameable_NV(std::move(name))
   {}
   virtual ~TestCase(void) = default;

   virtual DataShuttle run(DataShuttle const & InData = {}) = 0;
};

} // ym::ut
