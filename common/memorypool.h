/**
 * @file    memorypool.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "lightlogger.h"
#include "lightymception.h"

#include <memory>
#include <vector>

namespace ym
{

/**
 * TODO have lightlogger included for error reporting
 * TODO since we cannot cleanly have a destructor, we need
 *      to make it private, which means we need static instance,
 *      which means it needs to be thread safe. Am I missing
 *      something here?
 */
template <typename T>
class MemoryPool
{
public:
   explicit MemoryPool(uint64 const NChunksPerBlock);

   ~MemoryPool(void);

   YM_NO_COPY  (MemoryPool)
   YM_NO_ASSIGN(MemoryPool)

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
