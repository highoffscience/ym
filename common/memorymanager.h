/**
 * @file    memorymanager.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <memory>

namespace ym
{

/**
 *
 */
template <typename T>
class MemoryManager
{
public:
   T * allocate(void); // TODO make undefined for data types < sizeof(T*),
                       //      especially char *! Don't want users confusing
                       //      it with a printable string.
   T * allocate(uint64 const NElements);

private:
   /**
    * One element per chunk
    */
   union Chunk
   {
      Chunk * next_ptr;
      char    data[sizeof(decltype(next_ptr))];
   };

   static constexpr uint64 s_DefaultNChunksPerBlock = 4096ul;

   Chunk *      _nextFreeChunk_ptr;
   uint64 const _NChunksPerBlock;
};

/**
 * TODO will eventually be a template
 */
template <typename T>
T * MemoryManager::allocate(uint64 const NElements)
{
   // TODO assert NElements > 0ul
   // TODO assert _NChunksPerBlock

   auto const NChunks = ((NElements * sizeof(T)) + (sizeof(T*) - 1ul)) / sizeof(T*);

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

   auto * const data_Ptr = reinterpret_cast<char *>(_nextFreeChunk_ptr);
   // NElements -- sizeof(T) -- NChunks
   //    25           1            4  // (NElements + (sizeof(Chunk) / sizeof(T)) - 1ul) / sizeof(Chunk)
   //    25           2            7
   //    25           4            13
   //    25           8            25 // sizeof(Chunk) == sizeof(T) from here onwards
   //    25           12           25
   //    25           16           25
   _nextFreeChunk_ptr = _nextFreeChunk_ptr->next_ptr;
   return data_Ptr;
}

} // ym
