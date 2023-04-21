/**
 * @file    memio.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "memio.h"

// /** allocBlock
//  * 
//  * @brief Allocates and initializes block of memory.
//  * 
//  * @note Each unused chunk points to the next available chunk. The sentinel chunk is used
//  *       to point to the next block.
//  *          ___   ___   ___
//  *         |   | |   | |   |
//  *         |   V |   V |   V
//  *       --|-----|-----|---------
//  *       | 1   | 2   | 3   | 4...
//  *       ------------------------
//  * 
//  * @param NChunksPerBlock -- # of chunks to allocate per block.
//  * @param ChunkSize_bytes -- Size of chunks in bytes.
//  *
//  * @returns void * -- Pointer to beginning of block of memory.
//  */
// void * ym::MemIO::allocBlock(uint64 const NChunksPerBlock,
//                              uint64 const ChunkSize_bytes)
// {
//    // TODO should I add alignment to this? 16 bytes is default. <https://en.cppreference.com/w/cpp/memory/new/operator_new>
//    // size allocated may not be a multiple of sizeof(uintptr)
//    auto * const block_Ptr =
//       static_cast<uintptr *>(
//          ::operator new((NChunksPerBlock * ChunkSize_bytes) + sizeof(uintptr)));

//    union
//    {
//       uintptr * ptr;
//       uintptr   uint;
//    } curr{block_Ptr};

//    for (uint64 i = 0_u64; i < NChunksPerBlock; ++i)
//    { // set "next" pointer for each chunk
//       *curr.ptr  =  curr.uint + ChunkSize_bytes;
//        curr.uint = *curr.ptr;
//    }
//    *curr.ptr = 0_u64; // init sentinel to nullptr

//    return block_Ptr;
// }