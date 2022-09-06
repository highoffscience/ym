/**
 * @file    memorypool.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "lightymception.h"

#include <memory>

namespace ym
{

/**
 * TODO have lightlogger included for error reporting
 */
template <typename T>
class MemoryPool
{
public:
   MemoryPool(void);
   MemoryPool(uint64 const NChunksPerBlock);

   ~MemoryPool(void);

   YM_NO_COPY  (MemoryPool)
   YM_NO_ASSIGN(MemoryPool)

   static MemoryPool<T> * getInstancePtr(void);

   T * allocate(void);
   std::unique_ptr<T> allocate_safe(void);

   void deallocate(T * const data_Ptr);

   /**
    * 
    */
   template <uint32 NElements = 128>
   struct MediaryStr
   {
      static_assert(NElements >= sizeof(T *), "NElements must be at least sizeof(T *)");

      explicit constexpr MediaryStr(void) : data{'\0'} {}

      /*implicit*/ constexpr operator char * (void) { return data; }

      char data[NElements];
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

   static constexpr uint64 s_DefaultNChunksPerBlock = 1024ul;

   Chunk *      _nextFreeChunk_ptr;
   uint64 const _NChunksPerBlock;
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
     _NChunksPerBlock   {NChunksPerBlock}
{
   ymLightAssert(_NChunksPerBlock > 0ul, "Block size must be > 0");
}

/**
 * TODO don't let exceptions escape destructor - add cleanup() method
 */
template <typename T>
MemoryPool<T>::~MemoryPool(void)
{
}

/**
 * TODO with this method (treating it as singleton) removes easy control over
 *      setting manually the _NChunksPerBlock
 */
template <typename T>
auto MemoryPool<T>::getInstancePtr(void) -> MemoryPool<T> *
{
   static MemoryPool<T> instance;

   return &instance;
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
