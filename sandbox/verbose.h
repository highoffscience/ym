/**
 * @author Forrest Jablonski
 */

#pragma once

#include <bitset>
#include <semaphore>

struct VerbosityGroup
{
   typedef unsigned char T;

   enum VGroupSlot_T : T // TODO should be same type as VerbosityGroup slot and mask
   {
      Stepper,
      Battery
   };

   // atomic
   void addVGroup(T const Name, T const mask);

   unsigned char slot;
   unsigned char mask;
};

void foo(void)
{
   unsigned verbosities[] = {0u, 0u};

   

   auto const VStepperCmd         = VerbosityGroup(Stepper, 0x01u);
   auto const VStepperOpto        = VerbosityGroup(Stepper, 0x02u);
   auto const VStepperStateChange = VerbosityGroup(Stepper, 0x04u);

}

static inline std::atomic<unsigned> s_vGroupCounter = 1u;

unsigned atomicLeftShift(std::atomic<unsigned> * const a_Ptr);
