/**
 * @file    MemoryPool.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <cstdint>
#include <new>

namespace ym
{

/**
 * 
 */
class MemoryPool
{
public:
   YM_NO_DEFAULT(MemoryPool)

   /**
    * Chunk - memory segment that contains the desired data type, T
    * Piece - word sized memory segment
    * Block - memory segment containing the desired amount of Chunks plus a sentinel Piece
    */
   template <typename T>
   class Pool
   {
   public:
      explicit Pool(T *    const block_Ptr,
                    uint64 const NChunksPerBlock);

      T * allocate(void);

   private:
      T * const _originalBlock_Ptr;
      T *       _activeBlock_ptr;
      T *       _sentinel_ptr;
      T *       _nextFreeChunk_ptr;
   };

   template <typename T>
   Pool<T> getNewPool(uint64 const NChunksPerBlock);

   void * allocateBlock(uint64 const NChunksPerBlock,
                        uint64 const ChunkSize_bytes);
};

/**
 *
 */
template <typename T>
MemoryPool::Pool<T>::Pool(T *    const block_Ptr,
                          uint64 const NChunksPerBlock)
   : _originalBlock_Ptr {block_Ptr                  },
     _activeBlock_ptr   {block_Ptr                  },
     _sentinel_ptr      {block_Ptr + NChunksPerBlock},
     _nextFreeChunk_ptr {block_Ptr                  }
{
}

/**
 * @return T *
 */
template <typename T>
T * MemoryPool::Pool<T>::allocate(void)
{
   if (_nextFreeChunk_ptr == _sentinel_ptr)
   {
      auto const NChunksPerBlock = _sentinel_ptr - _activeBlock_ptr;

      _activeBlock_ptr   = allocateBlock(NChunksPerBlock, sizeof(T));
      _sentinel_ptr      = _activeBlock_ptr + NChunksPerBlock;
      _nextFreeChunk_ptr = _activeBlock_ptr;
   }

   union
   {
      T *   ptr;
      T * * ptrptr;
   } data{_nextFreeChunk_ptr};

   _nextFreeChunk_ptr = *data.ptrptr;

   return data.ptr;
}

/**
 * @param NChunksPerBlock
 * 
 * @return Pool<T>
 */
template <typename T>
auto MemoryPool::getNewPool<T>(uint64 const NChunksPerBlock) -> Pool<T>
{
   // TODO throw if NChunksPerBlock == 0ul

   static_assert(sizeof(T) >= sizeof(std::uintptr_t), "Type must be at least size of chunk (min size sizeof(uintptr))");



   auto * const originalBlock_Ptr = static_cast<T *>(allocateBlock(NChunksPerBlock, sizeof(T)));

   return Pool<T>(originalBlock_Ptr, NChunksPerBlock);
}


// ----------------- TODO -----------------

// /**
//  *
//  */
// template <typename T>
// T * MemoryPool::allocatePool<T>(uint64 const NChunksPerBlock)
// {
//     std::allocator<T> a;
//     _nextFreeChunk_ptr = a.allocate(NChunksPerBlock);
// }

/**
 *
 */
template <typename T>
T * MemoryPool::allocate<T>(T * const pool_Ptr)
{

}

// ----------------------------------------

/**
 *
 */
template <typename T>
T * MemoryPool<T>::allocate(void)
{
   static_assert(sizeof(T) >= sizeof(Chunk *), "Chunk is of insufficient size"  );
   static_assert(sizeof(T) == sizeof(Chunk  ), "Chunk and T should be same size");

   if (!_nextFreeChunk_ptr)
   {
      std::allocator<Chunk> a;
      _nextFreeChunk_ptr = a.allocate(_NChunksPerBlock);

      auto * chunk_ptr = _nextFreeChunk_ptr;
      for (uint64 i = 0ul; i < _NChunksPerBlock - 1ul; ++i)
      {
         chunk_ptr->next_ptr = chunk_ptr + 1ul;
         chunk_ptr = chunk_ptr->next_ptr;
      }
      chunk_ptr->next_ptr = nullptr;
   }

   auto * const data_Ptr = reinterpret_cast<T *>(_nextFreeChunk_ptr);
   _nextFreeChunk_ptr = _nextFreeChunk_ptr->next_ptr;
   return data_Ptr;
}

/**
 *
 */
template <typename T>
std::unique_ptr<T> MemoryPool<T>::allocate_safe(void)
{
   return std::unique_ptr<T, decltype(deallocate)>(data_Ptr, deallocate);
}

/**
 *
 */
template <typename T>
void MemoryPool<T>::deallocate(T * const data_Ptr)
{
   auto * const chunk_Ptr = reinterpret_cast<Chunk *>(data_Ptr);
   chunk_Ptr->next_ptr = _nextFreeChunk_ptr;
   _nextFreeChunk_ptr = chunk_Ptr;
}

} // ym
