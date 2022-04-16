/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym_unittest.h"

namespace ym::unittest
{

/**
 *
 */
class UnitTestBase
{
public:
   explicit UnitTestBase(void);
   virtual ~UnitTestBase(void) = default;

   virtual void runTests(void) = 0;
};

} // ym::unittest
