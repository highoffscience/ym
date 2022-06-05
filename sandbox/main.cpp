/**
 * @author Forrest Jablonski
 */

#include "battery.h"
#include "stepper.h"

#include <atomic>
#include <bitset>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <new>
#include <semaphore>

int main(void)
{
   sizeof(std::bitset<4096>);

   std::cout << std::endl;

   std::cout << StepperCmd  << std::endl;
   std::cout << StepperOpto << std::endl;

   std::cout << BatteryGauge   << std::endl;
   std::cout << BatteryCharger << std::endl;

   return 0;
}
