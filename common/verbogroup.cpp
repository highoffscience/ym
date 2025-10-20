/**
 * @file    verbogroup.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "verbogroup.h"

/**
 * @brief TODO
 * 
 * @param G 
 */
void ym::VerboGroup::set(Flags_T const F)
{
   YMASSERT(F != Flags_T::NFlags, Error, YM_DAH, "Invalid verbosity group");

   auto const I = std::to_underlying(F) / ymGetNBits<unsigned>();
   auto const M = std::to_underlying(F) % ymGetNBits<unsigned>();

   std::ignore = _flags[I].fetch_or(1u << M, std::memory_order_relaxed);
}

/**
 * @brief TODO
 * 
 * @param G 
 */
void ym::VerboGroup::clear(Flags_T const F)
{
   YMASSERT(F != Flags_T::NFlags, Error, YM_DAH, "Invalid verbosity group");

   auto const I = std::to_underlying(F) / ymGetNBits<unsigned>();
   auto const M = std::to_underlying(F) % ymGetNBits<unsigned>();

   std::ignore = _flags[I].fetch_and(~(1u << M), std::memory_order_relaxed);
}

/**
 * @brief TODO
 * 
 * @param G 
 */
bool ym::VerboGroup::test(Flags_T const F) const
{
   YMASSERT(F != Flags_T::NFlags, Error, YM_DAH, "Invalid verbosity group");

   auto const I = std::to_underlying(F) / ymGetNBits<unsigned>();
   auto const M = std::to_underlying(F) % ymGetNBits<unsigned>();

   return (_flags[I].load(std::memory_order_relaxed) & (1u << M)) != 0u;
}
