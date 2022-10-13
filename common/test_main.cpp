/**
 * @file    test_main.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "objectgroups.h"

#include <iostream>

int main(void)
{
   using namespace ym;

   auto const E = std::to_underlying(ym::ObjectGroupMask_T::Ymception_Assert);

   using T = long double;// __float80;
   T i = 3.1415;
   T j = 2.0;

   std::cout << sizeof(i) << " :: " << static_cast<float64>(i * j) << std::endl;

   return 0;
}
