/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym_unittest.h"

#include "random.h"
#include "unittestbase.h"

namespace ym::unittest
{

/**
 * 
 */
class Random_UnitTest : public UnitTestBase
{
public:
   explicit Random_UnitTest(void);
   virtual ~Random_UnitTest(void) = default;

   virtual void runTests(void) override;

private:
   void minMaxAveTest(void);
   void binTest(void);
   void piTest(void);

   // f32PiTest and f64PiTest

   Random _rand;
};

} // ym::unittest
