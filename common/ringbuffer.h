/**
 * @file    ringbuffer.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "memio.h"
#include "ymdefs.h"

#include <utility>

namespace ym
{

/** RingBuffer
 *
 * TODO
 */
template <typename T,
          uint64   Depth> // TODO static_assert or requires based on c++ standard (> 0 && < 2^31)
class RingBuffer
{
public:
   explicit inline RingBuffer(void);

   void reset(void);

   template <typename... Args_T>
   void emplace(Args_T... args);

private:
   struct IdxFlag
   {
      uint64 idx      : 63 {Depth - 1_u64};
      bool   rollover :  1 {false        };
   };
   static_assert(sizeof(IdxFlag) == sizeof(uint64), "Bit packing not as expected");

   alignas(T) uint8 _buffer[Depth * sizeof(T)] {};
   IdxFlag          _next                      {};
};

/**
 * @brief TODO
 * 
 */
template <typename T,
          uint64   Depth>
inline RingBuffer<T, Depth>::RingBuffer(void)
   : _buffer_Ptr {MemIO::alloc(Depth)}
{
   reset();
}

/**
 * @brief TODO
 * 
 */
template <typename T,
          uint64   Depth>
void RingBuffer<T, Depth>::reset(void)
{
   _next_idx = Depth - 1_u64;
}

/**
 * @brief TODO
 * 
 */
template <typename T,
          uint64   Depth>
template <typename... Args_T>
void RingBuffer<T, Depth>::emplace(Args_T... args)
{
   _next_idx = (((_next_idx & 0x7FFF'FFFF'FFFF'FFFF_u64) + 1_u64) % Depth) |
                 (_next_idx & 0x8000'0000'0000'0000_u64);

   if (_next_idx == 0_u64)
   { // rollover occured
      _next_idx |= 0x8000'0000'0000'0000_u64;
   }

   
}

} // ym
