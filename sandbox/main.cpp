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
   char buf[i];
   buf[i-1] = '\0';
   std::cout << buf << std::endl;
}

int main(void)
{
   std::cout << BUFSIZ << std::endl;

   test(10);

   return 0;
}
