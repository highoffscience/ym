/**
 * @file    test_main.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include <cstdint>
#include <iostream>

#include "ym.h"
#include "random.h"

int main(void)
{
   using namespace ym;

   auto r = Random();
   
   std::cout << r.gen<uint32>() << std::endl;

   return 0;
}
