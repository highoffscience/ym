/**
 * @author Forrest Jablonski
 */

#include "ym_unittest.h"

#include "unittestbase.h"
#include "random_unittest.h"

#include <cstdio>

int main(int const /*Argc*/, ym::str const * const /*Argv_Ptr*/)
{
   std::printf("Go! Torchic!\n");

   using namespace ym;
   using namespace ym::unittest;

   UnitTestBase * unitTest_ptr = new Random_UnitTest();
   unitTest_ptr->runTests();
   delete unitTest_ptr;
   unitTest_ptr = nullptr;

   return 0;
}
