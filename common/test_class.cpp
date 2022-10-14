/**
 * @file    test_main.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "test_main.h"

#include <iostream>

void foo(void)
{
   using namespace ym;

   Event<uint16> e0;
   Event<uint32> e1;
   Event<uint64> e2;

   std::cout << "e0:" << e0.getClassID_func() << " -- " << Event<uint16>::getClassID_func() << std::endl;
   std::cout << "e1:" << e1.getClassID_func() << " -- " << Event<uint32>::getClassID_func() << std::endl;
   std::cout << "e2:" << e2.getClassID_func() << " -- " << Event<uint64>::getClassID_func() << std::endl;
}
