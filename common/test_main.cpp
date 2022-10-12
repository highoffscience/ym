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

   std::cout << E << std::endl;

   return 0;
}
