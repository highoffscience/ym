/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "ymception.h"

#include <new>
#include <vector>

namespace ym
{

/*
 * Convenience functions.
 * -------------------------------------------------------------------------- */

template <typename T>
inline T * ymAllocate(uint64 const NElements);

inline void ymDeallocate(void * const data_Ptr);

/* -------------------------------------------------------------------------- */

/**
 *
 */
class MemoryDefs
{
public:
   YM_NO_DEFAULT(MemoryDefs)

   template <typename T>
   friend T * ymAllocate(uint64 const NElements);

   friend void ymDeallocate(void * const data_Ptr);

private:
   static T  * allocate  (uint64 const NElements,
                          uint32 const SizeOfType`);
   static void deallocate(void * const data_Ptr);

   struct PtrAndSize_T
   {
      void * const data_Ptr;
      uint64       size_bytes;
   };
   static std::vector<PtrAndSize_T> _allocatedPtrs;
};

/**
 *
 */
template <typename T>
inline T * ymAllocate(uint64 const NElements)
{
   return MemoryDefs::allocate(NElements, sizeof(T));
}

/**
 *
 */
inline void ymDeallocate(void * const data_Ptr)
{
   MemoryDefs::deallocate(data_Ptr);
}

/**
 *
 */
template <typename T>
T * MemoryDefs::allocate(uint64 const NElements)
{
   auto * const data_Ptr = static_cast<T *>(::operator new(NElements, std::align_val_t{sizeof(T)}));

   ymAssert(data_Ptr, "Alignment new failed!");



   std::allocator<T> a;
   T * data_ptr = nullptr;

   try
   {
      data_ptr = a.allocate(NElements);
      _allocatedSizes.insert({data_ptr, NElements * sizeof(T)});
   }
   catch(std::exception const & E)
   {
      data_ptr = nullptr;
      ymAssert(false, E.what());
   }

   return data_ptr;
}

} // ym
