/**
 * @file    test_main.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"

namespace ym
{

namespace ctypeid {
inline uint32 type_id_counter = 1;
template <typename T>
inline uint32 type_id = type_id_counter++;
}

template <typename T>
struct Event
{
   static constexpr uint32 getClassID_seq(void)
   {
      return ctypeid::type_id<T>;
   }

   static constexpr uint64 getClassID_func(void)
   {
      return 0; //ymPtrToUint(&getClassID_func);
   }
};

} // ym

void foo(void);
