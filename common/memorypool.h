/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <new>
#include <vector>

namespace ym
{

/**
 *
 */
template <typename T>
class MemoryPool
{
public:
   T * allocate(uint64 const NObjects);

   inline void free(T *    const data_Ptr);
          void free(T *    const data_Ptr,
                    uint64 const NObjects);

private:
   /**
    *
    */
   union Chunk
   {
      Chunk * next_ptr;
      T       data;
   };
   static_assert(sizeof(T) >= sizeof(Chunk *), "Chunk is of insufficient size");

   std::vector<Chunk *> _blocks;
   Chunk *              _nextFreeChunk_ptr;
};

/**
 * TODO if doing a range we need to guarantee these objects are consecutive in memory
 *
 * Range allocations can only be safely done in their own isolated blocks.
 *  Well actually maybe if I keep track of _prevFreeChunk_ptr,
 */
template <typename T>
T * MemoryPool<T>::allocate(uint64 const NObjects)
{
   T * data_ptr = nullptr;

   if (NObjects > 0)
   {
      data_ptr = static_cast<T *>(::operator new(NObjects * sizeof(T)));
   }

   return data_ptr;
}

/**
 *
 */
template <typename T>
inline void MemoryPool<T>::free(T * const data_Ptr)
{
   auto * chunk_ptr = reinterpret_cast<Chunk *>(data_Ptr);

   chunk_ptr->next_ptr = _nextFreeChunk_ptr;
   _nextFreeChunk_ptr = chunk_ptr;
}

/**
 * TODO range check ptr
 *
//  * TODO is this correct?
 */
template <typename T>
void MemoryPool<T>::free(T *    const data_Ptr,
                         uint64 const NObjects)
{
   if (NObjects > 0)
   {
      auto * chunk_ptr = reinterpret_cast<Chunk *>(data_Ptr);

      _nextFreeChunk_ptr = chunk_ptr;

      for (uint64 i = 0; i < NObjects; ++i)
      {
         chunk_ptr->next_ptr = chunk_ptr + i + 1;
         chunk_ptr = chunk_ptr->next_ptr;
      }
   }
}

} // ym
