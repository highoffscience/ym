/**
 * @file    testcase.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "nameable.h"
#include "ymdefs.h"

#include "datashuttle.h"

#include <string>

namespace ym::intg
{

/**
 * @brief Defines a test case.
 *
 * @param Name_ -- Name of test case.
 */
#define YM_UNIT_TESTCASE(Name_)                                          \
   class Name_ : public TestCase                                         \
   {                                                                     \
   public:                                                               \
      explicit Name_(void) noexcept : TestCase(#Name_) {}                \
      virtual DataShuttle run(DataShuttle const & InData = {}) override; \
   };

/**
 * @brief Represents a test case.
 */
class TestCase : public PermaNameable_NV<>
{
public:
   explicit inline TestCase(std::string name) noexcept :
      PermaNameable_NV(std::move(name))
   { }
   virtual ~TestCase(void) = default;

   virtual DataShuttle run(DataShuttle const & InData = {}) = 0;
};

} // ym::intg
