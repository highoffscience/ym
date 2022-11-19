/**
 * @file    test_main.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include <cstdint>
#include <iostream>
#include <thread>

#include "ym.h"
#include "timer.h"
#include "random.h"

int main(void)
{
   using namespace ym;

   ym::Timer t;

   std::this_thread::sleep_for(std::chrono::milliseconds(100));

   std::cout << t.getElapsedTime() << std::endl;

   auto r = Random();
   
   std::cout << r.gen<uint32>() << std::endl;

   return 0;
}
