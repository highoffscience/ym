/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <new>

namespace ym
{

/**
 *
 */
template <typename T>
class MemoryPool
{
public:
   // TODO why is this class a singleton?
   static MemoryPool * getInstancePtr(void);

   T * allocate(void);
   T * allocate(uint64 const NObjects);

   void free(T *    const data_Ptr);
   void free(T *    const data_Ptr,
             uint64 const NObjects);

private:
   MemoryPool(void);
   MemoryPool(uint64 const NChunksPerBlock);

   ~MemoryPool(void);

   YM_NO_COPY          (MemoryPool)
   YM_NO_MOVE_ASSIGN   (MemoryPool)
   YM_NO_MOVE_CONSTRUCT(MemoryPool)

   /**
    *
    */
   union Chunk
   {
      Chunk * next_ptr;
      T       data;
   };
   static_assert(sizeof(T) >= sizeof(Chunk *), "Chunk is of insufficient size");

   static constexpr uint64 s_DefaultNChunksPerBlock = 4096ul;

   Chunk *      _nextFreeChunk_ptr;
   uint64 const _NChunksPerBlock;
};

/**
 *
 */
template <typename T>
MemoryPool<T>::MemoryPool(void)
   : _nextFreeChunk_ptr {nullptr                 },
     _NChunksPerBlock   {s_DefaultNChunksPerBlock}
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
   // ymAssert(_NChunksPerBlock > 0ul, "Block size must be > 0");
}

/**
 * TODO
 */


/**
 *
 */
template <typename T>
T * MemoryPool<T>::allocate(void)
{
   if (!_nextFreeChunk_ptr)
   {
      _nextFreeChunk_ptr = static_cast<Chunk *>(::operator new(sizeof(Chunk) * _NChunksPerBlock));
      
      for (uint64 i = 0ul; i < _NChunksPerBlock; ++i)
      {
         (_nextFreeChunk_ptr + i)->next_ptr = _nextFreeChunk_ptr + i + 1ul;
      }
      (_nextFreeChunk_ptr + _NChunksPerBlock - 1ul)->next_ptr = nullptr;
   }

   auto * const data_Ptr = reinterpret_cast<T *>(_nextFreeChunk_ptr);
   _nextFreeChunk_ptr = _nextFreeChunk_ptr->next_ptr;
   return data_Ptr;
}

/**
 * TODO
 */
template <typename T>
T * MemoryPool<T>::allocate(uint64 const NObjects)
{
   return nullptr;
}

/**
 *
 */
template <typename T>
inline void MemoryPool<T>::free(T * const data_Ptr)
{
   auto * const chunk_Ptr = reinterpret_cast<Chunk *>(data_Ptr);
   chunk_Ptr->next_ptr = _nextFreeChunk_ptr;
   _nextFreeChunk_ptr = chunk_Ptr;
}

/**
 * TODO
 */
template <typename T>
void MemoryPool<T>::free(T *    const data_Ptr,
                         uint64 const NObjects)
{
}

} // ym
