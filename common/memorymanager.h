/**
 * @file    memorymanager.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <alloca.h>
#include <cstdint>
#include <new>

namespace ym
{

/// @brief Convenience alias.
using MemMan = class MemoryManager;

// TODO create concept for chunk

/**
 *
 */
class MemoryManager
{
public:
   YM_NO_DEFAULT(MemoryManager)

   /** Pool
    * 
    * @brief Class that manages a particular pool of memory.
    */
   template <typename T>
   class Pool
   {
   public:
      explicit Pool(uint64 const NChunksPerBlock);

      T * allocate(void);

   private:
      T *       _activeBlock_ptr;
      T *       _sentinel_ptr;
      T *       _nextFreeChunk_ptr;
      T * const _originalBlock_Ptr; // must be last
   };

   template <typename T>
   static Pool<T> getNewPool(uint64 const NChunksPerBlock);

   template <typename T>
   static inline T * stackAlloc(uint32 const NElements);

private:
   static void * allocateBlock(uint64 const NChunksPerBlock,
                               uint64 const ChunkSize_bytes);
};



/** Pool
 * 
 * @brief Constructor.
 * 
 * @tparam T -- Type the pool contains.
 * 
 * @param NChunksPerBlock -- Chunks (datum elements) per block of memory.
 */
template <typename T>
MemMan::Pool<T>::Pool(uint64 const NChunksPerBlock)
   : _activeBlock_ptr   {nullptr                           },
     _sentinel_ptr      {_activeBlock_ptr + NChunksPerBlock},
     _nextFreeChunk_ptr {_sentinel_ptr                     }, // forces allocation
     _originalBlock_Ptr {allocate<T>()                     },
{
}

/**
 * TODO
 */
template <typename T>
T * MemMan::Pool<T>::allocate(void)
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

/** stackAlloc
 *
 * @brief Allocates requested amount of bytes on the stack at runtime.
 *
 * @note Functionally moves the stack pointer to where you want. We mimic the
 *       behaviour of variable length arrays.
 *
 * @note Memory allocated by this function automatically gets freed when the
 *       embedding function goes out of scope.
 *
 * @ref <https://man7.org/linux/man-pages/man3/alloca.3.html>.
 * @ref <https://en.cppreference.com/w/c/language/array>. See section on VLA's.
 *
 * @tparam T -- Type to allocate
 *
 * @param NElements -- Number of T elements to allocate room for
 *
 * @return T * -- Pointer to newly allocated stack memory
 */
template <typename T>
inline T * MemPool::stackAlloc(uint32 const NElements)
{
   return static_cast<T *>(alloca(NElements * sizeof(T)));
}

} // ym
