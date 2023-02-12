/**
 * @file    memorymanager.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "ymception.h"

#include <alloca.h>
#include <cstdint>
#include <memory>

namespace ym
{

/** YM_STACK_ALLOC
 *
 * @brief Allocates requested amount of bytes on the stack at runtime.
 *
 * @note Functionally moves the stack pointer to where you want. We mimic the
 *       behaviour of variable length arrays.
 *
 * @note Memory allocated by this function automatically gets freed when the
 *       embedding function goes out of scope.
 * 
 * @note Only allocates memory in current stack frame, so this must a macro,
 *       not an inline function.
 *
 * @ref <https://man7.org/linux/man-pages/man3/alloca.3.html>.
 * @ref <https://en.cppreference.com/w/c/language/array>. See section on VLA's.
 *
 * @param Type_      -- Type to allocate.
 * @param NElements_ -- Number of T elements to allocate room for.
 *
 * @return Type_ * -- Pointer to newly allocated stack memory.
 */
#define YM_STACK_ALLOC(Type_, NElements_) \
   static_cast<Type_ *>(alloca(NElements_ * sizeof(Type_)));

/// @brief Convenience alias.
using MemMgr = class MemoryManager;

/** Chunkable_T
 *
 * @brief Represents one datum element in a memory pool.
 *
 * @tparam T -- Type that is chunkable.
 */
template <typename T>
concept Chunkable_T = (sizeof(T) >= sizeof(std::uintptr_t));

/** MemoryManager
 *
 * @brief Class that provides memory management utilities.
 */
class MemoryManager
{
public:
   YM_NO_DEFAULT(MemoryManager)

   YM_DECL_YMCEPT_BASE(MemoryManagerError)
   YM_DECL_YMCEPT(     MemoryManagerError, MemoryManagerError_InvalidNChunks)

   /** Pool
    *
    * @brief Class that manages a particular pool of memory.
    */
   template <Chunkable_T Chunk_T>
   class Pool
   {
   public:
      explicit Pool(uint64 const NChunksPerBlock);

      Chunk_T *                allocate     (void);
      std::unique_ptr<Chunk_T> allocate_safe(void);

      void deallocate(Chunk_T * const datum_Ptr);

   private:
      Chunk_T *       _activeBlock_ptr;
      Chunk_T *       _sentinel_ptr;
      Chunk_T *       _nextFreeChunk_ptr;
      Chunk_T * const _originalBlock_Ptr; // must be last
   };

   template <Chunkable_T Chunk_T>
   static Pool<Chunk_T> getNewPool(uint64 const NChunksPerBlock);

private:
   static void * allocateBlock(uint64 const NChunksPerBlock,
                               uint64 const ChunkSize_bytes);
};

/** Pool
 *
 * @brief Constructor.
 *
 * @tparam Chunk_T -- Type the pool contains.
 *
 * @param NChunksPerBlock -- Chunks (datum elements) per block of memory.
 */
template <Chunkable_T Chunk_T>
MemMgr::Pool<Chunk_T>::Pool(uint64 const NChunksPerBlock)
   : _activeBlock_ptr   {nullptr                           },
     _sentinel_ptr      {_activeBlock_ptr + NChunksPerBlock},
     _nextFreeChunk_ptr {_sentinel_ptr                     }, // forces allocation
     _originalBlock_Ptr {allocate<Chunk_T>()               }
{
}

/** allocate
 *
 * @brief Returns a pointer to a block of raw memory equal to sizeof(Chunk_T).
 *
 * @tparam Chunk_T -- Type the pool contains.
 * 
 * @return Pointer to block of raw memory equal to sizeof(Chunk_T).
 */
template <Chunkable_T Chunk_T>
Chunk_T * MemMgr::Pool<Chunk_T>::allocate(void)
{
   if (_nextFreeChunk_ptr == _sentinel_ptr)
   { // current pool is exhausted - create another one
      auto const NChunksPerBlock = _sentinel_ptr - _activeBlock_ptr;

      _activeBlock_ptr   = allocateBlock(NChunksPerBlock, sizeof(Chunk_T));
      _sentinel_ptr      = _activeBlock_ptr + NChunksPerBlock;
      _nextFreeChunk_ptr = _activeBlock_ptr;
   }

   union
   {
      Chunk_T *   ptr;
      Chunk_T * * ptrptr;
   } data{_nextFreeChunk_ptr};

   _nextFreeChunk_ptr = *data.ptrptr;

   return data.ptr;
}

// TODO
// /** allocate_safe
//  *
//  * @brief Returns a smart pointer to a block of raw memory equal to sizeof(Chunk_T).
//  * 
//  * @tparam Chunk_T -- Type the pool contains.
//  * 
//  * @return Smart pointer to block of raw memory equal to sizeof(Chunk_T).
//  */
// template <Chunkable_T Chunk_T>
// std::unique_ptr<Chunk_T> MemMan::Pool<Chunk_T>::allocate_safe(void)
// {
//    template<typename Chunk_T>
//    auto deleter = [this](Chunk_T * const datum_Ptr) {

//    };

//    return std::unique_ptr<Chunk_T, decltype(deleter)>(allocate(), &deallocate<Chunk_T>);
// }

/** deallocate
 *
 * @brief Frees chunk of memory to the pool.
 * 
 * @tparam Chunk_T -- Type the pool contains.
 * 
 * @param datum_Ptr -- Chunk of memory to free.
 */
template <Chunkable_T Chunk_T>
void MemMgr::Pool<Chunk_T>::deallocate(Chunk_T * const datum_Ptr)
{
   *reinterpret_cast<Chunk_T * *>(datum_Ptr) = _nextFreeChunk_ptr;
   _nextFreeChunk_ptr = datum_Ptr;
}

/** getNewPool
 *
 * @brief Creates and returns a new memory pool.
 *
 * @tparam Chunk_T -- Type the pool contains.
 * 
 * @param NChunksPerBlock -- Number of chunks (datum elements) per block of memory.
 *
 * @return Pool<Chunk_T> -- Pool object.
 */
template <Chunkable_T Chunk_T>
auto MemMgr::getNewPool(uint64 const NChunksPerBlock) -> Pool<Chunk_T>
{
   ymAssert<MemoryManagerError_InvalidNChunks>(NChunksPerBlock > 0_u64,
      "# of chunks must be greater than 0");

   auto * const originalBlock_Ptr =
      static_cast<Chunk_T *>(allocateBlock(NChunksPerBlock, sizeof(Chunk_T)));

   return Pool<Chunk_T>(originalBlock_Ptr, NChunksPerBlock);
}

} // ym
