/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "ymception.h"

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

   YM_NO_COPY  (MemoryPool)
   YM_NO_ASSIGN(MemoryPool)

   static auto /*unique_ptr*/ allocate(uint64 const NElements);

   T * allocate(void);
   void deallocate(T * const data_Ptr);

private:
   /**
    * One element per chunk
    */
   union Chunk
   {
      Chunk * next_ptr;
      T       data;
   };

   static constexpr uint64 s_DefaultNChunksPerBlock = 4096ul;

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
   ymAssert(_NChunksPerBlock > 0ul, "Block size must be > 0");
}

/**
 * TODO deallocate chunks
 *      Only deallocate free chunks, the others are being used by clients
 */
template <typename T>
MemoryPool<T>::~MemoryPool(void)
{
}

/**
 *
 */
template <typename T>
auto MemoryPool<T>::allocate(uint64 const NElements)
{
   std::allocator<T> a;
   auto * const data_Ptr = (NElements > 0ul) ? a.allocate(NElements) : nullptr;
   auto deleter = [&](T * const data_Ptr) { a.deallocate(data_Ptr, NElements); };
   return std::unique_ptr<T, decltype(deleter)>(data_Ptr, deleter);
}

/**
 * We are guaranteed _NChunksPerBlock > 0ul
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
void MemoryPool<T>::deallocate(T * const data_Ptr)
{
   auto * const chunk_Ptr = reinterpret_cast<Chunk *>(data_Ptr);
   chunk_Ptr->next_ptr = _nextFreeChunk_ptr;
   _nextFreeChunk_ptr = chunk_Ptr;
}

/**
 *
 */
template <typename T>
void MemoryPool<T>::deallocate(std::unique_ptr<T> & data_uptr_ref,
                               uint64 const NObjects)
{
   if (data_uptr_ref)
   {
      std::allocator<T> a;
      a.deallocate(data_uptr_ref.release(), NObjects);
   }
}

} // ym
