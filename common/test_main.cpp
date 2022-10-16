/**
 * @file    test_main.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "test_main.h"

#include "ymception.h"

#include <cstdint>
#include <iostream>

int test_seq(int i, int j);

int main(void)
{
   using namespace ym;

   ymAssert<Ymception>(false, "test main failure id %u", 9);

   return 0;
}
