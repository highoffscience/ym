/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <memory>

namespace ym
{

/**
 *
 */
template <typename T>
class MemoryPool
{
public:
   MemoryPool(void);
   MemoryPool(uint64 const NChunksPerBlock);

   ~MemoryPool(void);

   YM_NO_COPY          (MemoryPool)
   YM_NO_MOVE_ASSIGN   (MemoryPool)
   YM_NO_MOVE_CONSTRUCT(MemoryPool)

   T * allocate(void);
   T * allocate(uint64 const NObjects);

   void free(T *    const data_Ptr);
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

   static_assert(sizeof(T) >= sizeof(Chunk *), "Chunk is of insufficient size"  );
   static_assert(sizeof(T) == sizeof(Chunk  ), "Chunk and T should be same size");

   static constexpr uint64 s_DefaultNChunksPerBlock = 4096ul;

   Chunk *           _nextFreeChunk_ptr;
   uint64 const      _NChunksPerBlock;
   std::allocator<T> _allocator;
};

/**
 *
 */
template <typename T>
MemoryPool<T>::MemoryPool(void)
   : MemoryPool(s_DefaultNChunksPerBlock)
{
}

/**
 * TODO
 */
template <typename T>
MemoryPool<T>::MemoryPool(uint64 const NChunksPerBlock)
   : _nextFreeChunk_ptr {nullptr        },
     _NChunksPerBlock   {NChunksPerBlock},
     _allocator         {               }
{
   // ymAssert(_NChunksPerBlock > 0ul, "Block size must be > 0");
}

/**
 * TODO
 */
template <typename T>
MemoryPool<T>::~MemoryPool(void)
{
}

/**
 * TODO log exceptions and return null maybe?
 */
template <typename T>
T * MemoryPool<T>::allocate(void)
{
   auto * data_ptr = reinterpret_cast<T *>(_nextFreeChunk_ptr);

   if (!data_ptr)
   {
      data_ptr = _allocator.allocate(_NChunksPerBlock);
      _nextFreeChunk_ptr = reinterpret_cast<Chunk *>(data_ptr);

      for (uint64 i = 0ul; i < _NChunksPerBlock; ++i)
      {
         (_nextFreeChunk_ptr + i)->next_ptr = _nextFreeChunk_ptr + i + 1ul;
      }
      (_nextFreeChunk_ptr + _NChunksPerBlock - 1ul)->next_ptr = nullptr;
   }

   _nextFreeChunk_ptr = _nextFreeChunk_ptr->next_ptr;
   return data_ptr;
}

/**
 * TODO log exceptions and return null maybe?
 */
template <typename T>
T * MemoryPool<T>::allocate(uint64 const NObjects)
{
   return _allocator.allocate(NObjects);
}

/**
 *
 */
template <typename T>
void MemoryPool<T>::free(T * const data_Ptr)
{
   auto * const chunk_Ptr = reinterpret_cast<Chunk *>(data_Ptr);
   chunk_Ptr->next_ptr = _nextFreeChunk_ptr;
   _nextFreeChunk_ptr = chunk_Ptr;
}

/**
 *
 */
template <typename T>
void MemoryPool<T>::free(T *    const data_Ptr,
                         uint64 const NObjects)
{
   _allocator.deallocate(data_Ptr, NObjects);
}

} // ym
