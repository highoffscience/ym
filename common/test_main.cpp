/**
 * @file    test_main.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include <cstdint>
#include <iostream>
#include <bit>
#include <bitset>

#include "ym.h"
#include "random.h"

/*

0000 - 0
0001 - 1
0010 - 1
0011 - 2
0100 - 1
0101 - 2
0110 - 2
0111 - 3
1000 - 1
1001 - 2
1010 - 2
1011 - 3
1100 - 2
1101 - 3
1110 - 3
1111 - 4

*/

int main(void)
{
   using namespace ym;

   auto b = std::bitset<64>(127ull);

   auto r = Random();
   
   std::cout << r.gen<uint32>() << std::endl;

   return 0;
}
