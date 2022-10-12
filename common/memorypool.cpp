/**
 * @file    MemoryPool.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "memorypool.h"

/**
 * @param NChunksPerBlock
 * @param ChunkSize_bytes
 *
 * @return void *
 */
void * MemoryPool::allocateBlock(uint64 const NChunksPerBlock,
                                 uint64 const ChunkSize_bytes)
{
   auto * const block_Ptr =
      static_cast<uintptr *>(
         ::operator new(NChunksPerBlock * ChunkSize_bytes + sizeof(uintptr)));

   union
   {
      uintptr * ptr;
      uintptr   uint;
   } curr{block_Ptr};

   for (uint64 i = 0ul; i < NChunksPerBlock; ++i)
   {
      *curr.ptr  =  curr.uint + ChunkSize_bytes;
         curr.uint = *curr.ptr;
   }
   *curr.ptr = 0ul; // init sentinel to nullptr

   return block_Ptr;
}
