/**
 * @file    test_main.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include <cstdint>
#include <iostream>

#include "textlogger.h"
#include "verbositygroup.h"

int main(void)
{
   using namespace ym;

   ymLog(VGM_T::Logger, "Go! Torchic!");

   return 0;
}
