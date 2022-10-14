/**
 * @file    test_main.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "test_main.h"

#include <cstdint>
#include <iostream>

int test_seq(int i, int j);

int main(void)
{
   using namespace ym;

   Event<uint32> e1;
   Event<uint64> e2;

   // int sum = 3;

   // for (unsigned int i = 0u; i < 1'000'000'000u; ++i)
   // {
   //    if ((i*51u)%2u == 0u)
   //    {
   //       sum += test_seq(e1.getClassID_seq(), sum);
   //    }
   //    else
   //    {
   //       sum += test_seq(e2.getClassID_seq(), sum);
   //    }
   // }

   // std::cout << "sum is " << sum << std::endl;

   //auto const M = reinterpret_cast<uint64>(&main);

   foo();
   std::cout << std::endl;

   std::cout << "e1:" << e1.getClassID_func() << " -- " << Event<uint32>::getClassID_func() << std::endl;
   std::cout << "e2:" << e2.getClassID_func() << " -- " << Event<uint64>::getClassID_func() << std::endl;

   return 0;
}

int test_seq(int i, int j)
{
   switch (i)
   {
      // case 54567456: {
      //    return j + 3;
      // }
      // case 114231: {
      //    return j + 7;
      // }
      // case 10: {
      //    return j + 11;
      // }
      // case 6004: {
      //    return j + 13;
      // }
      // case 204742980: {
      //    return j + 17;
      // }
      // case 1204742980: {
      //    return j + 54;
      // }
      case 545657456: {
         return j + 3564;
      }
      case 1144231: {
         return j + 234;
      }
      // case 11: {
      //    return j + 12;
      // }
      // case 60704: {
      //    return j + 98;
      // }
      // case 20474980: {
      //    return j + 167;
      // }
      // case 1304742980: {
      //    return j + 546;
      // }

      case 0: {
         return j + 3;
      }
      case 1: {
         return j + 7;
      }
      default: {
         return j;
      }
   }
}
