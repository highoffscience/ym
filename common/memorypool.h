/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

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
 */
template <typename T>
T * MemoryPool<T>::allocate(uint64 const NObjects)
{
   T * data_ptr = nullptr;

   if (NObjects > 0)
   {
      if ()
   }

   return data_ptr;
}

/**
 *
 */
template <typename T>
inline void MemoryPool<T>::free(T * const data_Ptr)
{
   free(data_Ptr, 1);
}

/**
 *
 */
template <typename T>
void MemoryPool<T>::free(T *    const data_Ptr,
                         uint64 const NObjects)
{
   if (NObjects > 0ul)
   {
      auto * chunk_ptr = reinterpret_cast<Chunk *>(data_Ptr);

      _nextFreeChunk_ptr = chunk_ptr;

      for (uint64 i = 0ul; i < NObjects; ++i)
      {
         data_Ptr->next_ptr = data_Ptr + i + 1ul;
         data_Ptr = data_Ptr->next_ptr;
      }
   }
}

} // ym
