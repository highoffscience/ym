/**
 * @file    verbogroup.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "verbogroup.h"

#include <tuple> // some standards don't have std::ignore in <utility>

/**
 * @brief Sets the specified flag (flips to 1).
 *
 * @param F -- Flag to set.
 */
void ym::VerboGroup::set(Flag_T const F) noexcept
{
   auto const I = std::to_underlying(F) / ym_getNBits<unsigned>();
   auto const M = std::to_underlying(F) % ym_getNBits<unsigned>();

   std::ignore = _flags[I].fetch_or(1u << M, std::memory_order_relaxed);
}

/**
 * @brief Clears the specified flag (flips to 0).
 *
 * @param F -- Flag to clear.
 */
void ym::VerboGroup::clear(Flag_T const F) noexcept
{
   auto const I = std::to_underlying(F) / ym_getNBits<unsigned>();
   auto const M = std::to_underlying(F) % ym_getNBits<unsigned>();

   std::ignore = _flags[I].fetch_and(~(1u << M), std::memory_order_relaxed);
}

/**
 * @brief Test if the specified flag is enabled or not.
 *
 * @param F -- Flag to test.
 *
 * @returns bool -- True if flag is set, false otherwise.
 */
bool ym::VerboGroup::test(Flag_T const F) const noexcept
{
   auto const I = std::to_underlying(F) / ym_getNBits<unsigned>();
   auto const M = std::to_underlying(F) % ym_getNBits<unsigned>();

   return (_flags[I].load(std::memory_order_relaxed) & (1u << M)) != 0u;
}
