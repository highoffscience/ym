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
void ym::VerboGroup::set(Groups_T const G)
{
   YMASSERT(G != Groups_T::NGroups, Error, YM_DAH, "Invalid verbosity group");

   auto const I = std::to_underlying(G) / ymGetNBits<unsigned>();
   auto const M = std::to_underlying(G) % ymGetNBits<unsigned>();

   std::ignore = _flags[I].fetch_or(1u << M, std::memory_order_relaxed);
}

/**
 * @brief TODO
 * 
 * @param G 
 */
void ym::VerboGroup::clear(Groups_T const G)
{
   YMASSERT(G != Groups_T::NGroups, Error, YM_DAH, "Invalid verbosity group");

   auto const I = std::to_underlying(G) / ymGetNBits<unsigned>();
   auto const M = std::to_underlying(G) % ymGetNBits<unsigned>();

   std::ignore = _flags[I].fetch_and(~(1u << M), std::memory_order_relaxed);
}

/**
 * @brief TODO
 * 
 * @param G 
 */
bool ym::VerboGroup::test(Groups_T const G) const
{
   YMASSERT(G != Groups_T::NGroups, Error, YM_DAH, "Invalid verbosity group");

   auto const I = std::to_underlying(G) / ymGetNBits<unsigned>();
   auto const M = std::to_underlying(G) % ymGetNBits<unsigned>();

   return (_flags[I].load(std::memory_order_relaxed) & (1u << M)) != 0u;
}
