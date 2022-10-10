/**
 * @author Forrest Jablonski
 */

#include "ym_unittest.h"

#include "unittestbase.h"

#include "random_unittest.h"

#include <cstdio>

int main(void)
{
   ym::unittest::UnitTestBase * unitTestPtrs[] =
   {
      new ym::unittest::Random_UnitTest
   };

   for (auto * unitTest_ptr : unitTestPtrs)
   {
      unitTest_ptr->runTests();
      delete unitTest_ptr;
   }

   return 0;
}
