/**
 * @author Forrest Jablonski
 */

#include "unittestbase.h"

#include <iostream>

int main(void)
{
   ym::ut::UnitTestBase utb("Torchic");

   std::cout << utb.getName() << std::endl;

   return 0;
}
