/**
 * @file    memio.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "ymception.h"

#include <alloca.h>
#include <cstdint>
#include <memory>
#include <new>

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
 * @returns Type_ * -- Pointer to newly allocated stack memory.
 */
#define YM_STACK_ALLOC(Type_, NElements_) \
   static_cast<Type_ *>(alloca(NElements_ * sizeof(Type_)));

/** Chunkable_T
 *
 * @brief Represents one datum element in a memory pool.
 *
 * @tparam T -- Type that is chunkable.
 */
template <typename T>
concept Chunkable_T = (sizeof(T) >= sizeof(uintptr));

/** MemIO
 *
 * @brief Class that provides memory management utilities.
 */
class MemIO
{
public:
   YM_NO_DEFAULT(MemIO)

   YM_DECL_YMCEPT(MemIOError)
   YM_DECL_YMCEPT(MemIOError, MemIOError_PoolError)

   template <typename T>
   static inline T * alloc_raw(uint64 const NElements);

   template <typename T>
   static inline void dealloc_raw(T *    const data_Ptr,
                                  uint64 const NElements);

   template <typename T>
   static inline std::unique_ptr<T> alloc_safe(uint64 const NElements);

   // TODO
   // /** Pool
   //  *
   //  * @brief Class that manages a particular pool of memory.
   //  */
   // template <Chunkable_T Chunk_T>
   // class Pool
   // {
   // public:
   //    explicit Pool(uint64 const NChunksPerBlock);

   //    Chunk_T *                alloc_raw (void);
   //    std::unique_ptr<Chunk_T> alloc_safe(void);

   //    void dealloc(Chunk_T * const datum_Ptr);

   // private:
   //    Chunk_T *       _activeBlock_ptr;
   //    Chunk_T *       _sentinel_ptr;
   //    Chunk_T *       _nextFreeChunk_ptr;
   //    Chunk_T * const _originalBlock_Ptr; // must be last
   // };

   // template <Chunkable_T Chunk_T>
   // static Pool<Chunk_T> getNewPool(uint64 const NChunksPerBlock);

private:
   // static void * allocBlock(uint64 const NChunksPerBlock,
   //                          uint64 const ChunkSize_bytes);
};

/** alloc_raw
 * 
 * @brief Allocates raw chunk of memory.
 * 
 * @tparam T -- Type of data to allocate for.
 * 
 * @param NElements -- Number of elements to allocate for.
 * 
 * @returns T * -- Pointer to allocated memory.
 */
template <typename T>
inline T * MemIO::alloc_raw(uint64 const NElements)
{
   std::allocator<T> a;
   return a.allocate(NElements);
}

/**
 * TODO
 */
template <typename T>
inline void MemIO::dealloc_raw(T *    const data_Ptr,
                               uint64 const NElements)
{
   std::allocator<T> a;
   a.deallocate(data_Ptr, NElements);
}

/** alloc_safe
 * 
 * @brief Allocates raw chunk of memory wrapped in a smart pointer.
 * 
 * @tparam T -- Type of data to allocate for.
 * 
 * @param NElements -- Number of elements to allocate for.
 * 
 * @returns T * -- Smart pointer to allocated memory.
 */
template <typename T>
inline std::unique_ptr<T> MemIO::alloc_safe(uint64 const NElements)
{
   auto deleter = [NElements](T * const datum_Ptr) {
      dealloc_raw<T>(datum_Ptr, NElements);
   };

   return
      std::unique_ptr<T[], decltype(deleter)>(
         alloc_raw<T>(NElements), &deleter);
}

// /** Pool
//  *
//  * @brief Constructor.
//  *
//  * @tparam Chunk_T -- Type the pool contains.
//  *
//  * @param NChunksPerBlock -- Chunks (datum elements) per block of memory.
//  */
// template <Chunkable_T Chunk_T>
// MemIO::Pool<Chunk_T>::Pool(uint64 const NChunksPerBlock)
//    : _activeBlock_ptr   {nullptr                           },
//      _sentinel_ptr      {_activeBlock_ptr + NChunksPerBlock},
//      _nextFreeChunk_ptr {_sentinel_ptr                     }, // forces allocation
//      _originalBlock_Ptr {alloc_raw<Chunk_T>()              }
// {
// }

// /** alloc
//  *
//  * @brief Returns a pointer to a block of raw memory equal to sizeof(Chunk_T).
//  *
//  * @tparam Chunk_T -- Type the pool contains.
//  * 
//  * @returns Pointer to block of raw memory equal to sizeof(Chunk_T).
//  */
// template <Chunkable_T Chunk_T>
// Chunk_T * MemIO::Pool<Chunk_T>::alloc_raw(void)
// {
//    if (_nextFreeChunk_ptr == _sentinel_ptr)
//    { // current pool is exhausted - create another one
//       auto const NChunksPerBlock = _sentinel_ptr - _activeBlock_ptr;

//       _activeBlock_ptr   = allocBlock(NChunksPerBlock, sizeof(Chunk_T));
//       _sentinel_ptr      = _activeBlock_ptr + NChunksPerBlock;
//       _nextFreeChunk_ptr = _activeBlock_ptr;
//    }

//    union
//    {
//       Chunk_T *   ptr;
//       Chunk_T * * ptrptr;
//    } data{_nextFreeChunk_ptr};

//    _nextFreeChunk_ptr = *data.ptrptr;

//    return data.ptr;
// }

// /** alloc_safe
//  *
//  * @brief Returns a smart pointer to a block of raw memory equal to sizeof(Chunk_T).
//  * 
//  * @tparam Chunk_T -- Type the pool contains.
//  * 
//  * @returns Smart pointer to block of raw memory equal to sizeof(Chunk_T).
//  */
// template <Chunkable_T Chunk_T>
// std::unique_ptr<Chunk_T> MemIO::Pool<Chunk_T>::alloc_safe(void)
// {
//    // TODO
// }

// /** dealloc
//  *
//  * @brief Frees chunk of memory to the pool.
//  * 
//  * @tparam Chunk_T -- Type the pool contains.
//  * 
//  * @param datum_Ptr -- Chunk of memory to free.
//  */
// template <Chunkable_T Chunk_T>
// void MemIO::Pool<Chunk_T>::dealloc(Chunk_T * const datum_Ptr)
// {
//    *reinterpret_cast<Chunk_T * *>(datum_Ptr) = _nextFreeChunk_ptr;
//    _nextFreeChunk_ptr = datum_Ptr;
// }

// /** getNewPool
//  *
//  * @brief Creates and returns a new memory pool.
//  *
//  * @tparam Chunk_T -- Type the pool contains.
//  * 
//  * @param NChunksPerBlock -- Number of chunks (datum elements) per block of memory.
//  *
//  * @returns Pool<Chunk_T> -- Pool object.
//  */
// template <Chunkable_T Chunk_T>
// auto MemIO::getNewPool(uint64 const NChunksPerBlock) -> Pool<Chunk_T>
// {
//    MemIOError::check(NChunksPerBlock > 0_u64, "# of chunks must be greater than 0");

//    auto * const originalBlock_Ptr =
//       static_cast<Chunk_T *>(allocBlock(NChunksPerBlock, sizeof(Chunk_T)));

//    return Pool<Chunk_T>(originalBlock_Ptr, NChunksPerBlock);
// }

} // ym
