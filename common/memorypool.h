/**
 * @file    MemoryPool.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "lightlogger.h"
#include "lightymception.h"

#include <memory>

namespace ym
{

/**
 * TODO have lightlogger included for error reporting
 * TODO since we cannot cleanly have a destructor, we need
 *      to make it private, which means we need static instance,
 *      which means it needs to be thread safe. Am I missing
 *      something here?
 */
class MemoryPool
{
public:
   YM_NO_DEFAULT(MemoryPool)

   typedef uint64 U;

   /**
    * Chunk - memory segment that contains the desired data type, T
    * Piece - word sized memory segment
    * Block - memory segment containing the desired amount of Chunks plus a sentinel Piece
    */
   template <typename T>
   struct Pool
   {
      T * const _startingBlock_Ptr;
      U *       _sentinelPiece_ptr;
      T *       _nextFreeChunk_ptr;
   };

   template <typename T>
   Pool<T> * getNewPool(uint64 const NChunksPerBlock)
   {
      // TODO throw if NChunksPerBlock == 0ul

      static_assert(sizeof(U *) == sizeof(U), "Pointer must fit perfectly in Piece size");
      static_assert(sizeof(T  ) >= sizeof(U), "Type must be size of chunk or greater");

      auto * const startingBlock_Ptr = allocateBlock(NChunksPerBlock, sizeof(T));
      auto * const sentinelChunk_Ptr = startingBlock_Ptr + NChunksPerBlock;
      auto * const nextFreeChunk_Ptr = startingBlock_Ptr;

      Pool<T> pool{startingBlock_Ptr, sentinelChunk_Ptr, nextFreeChunk_Ptr};
   }

   uint64 * allocateBlock(uint64 const NChunksPerBlock,
                          uint64 const ChunkSize_bytes)
   {
      // calc how many Pieces can hold NChunks plus a sentinel Piece
      auto const NPiecesPerBlock = ((NChunksPerBlock * ChunkSize_bytes) + (sizeof(U) + 1ul)) / sizeof(U);

      std::allocator<U> a;
      auto * const block_Ptr = a.allocate(NPiecesPerBlock);

      auto * curr_ptr = block_Ptr;
      for (uint64 i = 0ul; i < NElements; ++i)
      {
         curr_ptr->next_ptr = chunk_ptr + 1ul;
         chunk_ptr = chunk_ptr->next_ptr;
      }

      return block_Ptr;
   }

   template <typename T>
   T * allocate(Pool<T> * const pool_Ptr)
   {
      if (pool_Ptr->_nextFreeChunk_ptr == pool_Ptr->_sentinelChunk_ptr)
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
   }

   T * allocatePool(uint64 const NChunksPerBlock);

   T * allocate(void);
   std::unique_ptr<T> allocate_safe(void);

   void deallocate(T * const data_Ptr);

   bool deallocateAll(void);

   /**
    * 
    */
   template <typename T,
             uint32   NElements = 128>
   struct ByteSegment
   {
      static_assert(sizeof(T) == 1ul, "Byte segments can only contain bytes");
      static_assert(NElements >= sizeof(T *), "NElements must be at least sizeof(T *)");

      explicit constexpr ByteSegment(void) : data{0} {}

      /*implicit*/ constexpr operator T * (void) { return data; }

      T data[NElements];
   };

private:
   /**
    * One element per chunk
    */
   union Chunk
   {
      Chunk * next_ptr;
      T       data;
   };

   Chunk *      _nextFreeChunk_ptr;
   uint64 const _NChunksPerBlock;
};

/**
 * TODO
 */
template <typename T>
MemoryPool<T>::MemoryPool(uint64 const NChunksPerBlock)
   : _nextFreeChunk_ptr {nullptr        },
     _NChunksPerBlock   {NChunksPerBlock}
{
   ymLightAssert(_NChunksPerBlock > 0ul, "Block size must be > 0");
}

/**
 * TODO don't let exceptions escape destructor - add cleanup() method
 * TODO add more robust handling and error reporting
 */
template <typename T>
MemoryPool<T>::~MemoryPool(void)
{
   if (!deallocateAll())
   { // some chunks are still being used by the client - this is a memory leak!
      LightLogger::getGlobalInstance()->printf("Some chunks are still in use!");
   }
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
