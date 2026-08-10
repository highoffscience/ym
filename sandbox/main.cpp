/**
 * @author Forrest Jablonski
 */

// #include "battery.h"
// #include "stepper.h"

#include <atomic>
#include <bitset>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <new>
#include <semaphore>

void test(int i)
{
   // throw std::runtime_error("Yo");
   auto e = std::runtime_error("Yo");
}

struct Point {
   int x, y;
};

int main(void)
{
   Point p{.x=1, .y=2};
   // try
   // {
   //    test(9);
   // }
   // catch (std::exception const & E)
   // {
   //    std::cout << E.what() << std::endl;
   // }

   return 0;
}
