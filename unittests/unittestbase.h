/**
 * @file    unittestbase.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym_ut.h"

namespace ym::ut
{

/** UnitTestBase
 *
 * @brief Base class for unit test suites.
 */
class UnitTestBase
{
public:
   explicit UnitTestBase(void);
   virtual ~UnitTestBase(void) = default;

   virtual void runTests(void) = 0;
};

} // ym::ut
