/**
 * @file    test_main.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include <cstdint>
#include <iostream>

#include "ym.h"

int main(void)
{
   using namespace ym;

   static_assert(sizeof(std::conditional_t<std::is_void_v<double>, uint8[8], double>) == 8, "uh-oh?");

   ym::float80 f = 19.5;
   auto t = ymPtrToUint(&f);
   
   std::cout << std::hex << t << std::endl;

   return 0;
}
