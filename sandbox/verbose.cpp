/**
 * @author Forrest Jablonski
 */

#include "verbose.h"

unsigned atomicLeftShift(std::atomic<unsigned> * const a_Ptr)
{
   auto val = a_Ptr->load(std::memory_order_acquire);
   while (!a_Ptr->compare_exchange_weak(val, val << 1u,
                                        std::memory_order_release,
                                        std::memory_order_relaxed));
   return val;
}
