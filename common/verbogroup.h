/**
 * @file    verbogroup.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include <array>
#include <atomic>
#include <cmath>
#include <type_traits>
#include <utility>

namespace ym
{

/** VerboGroup
 *
 * @brief Top-level group of registered objects (verbosity group).
 *
 * @note This is a wrapped scoped enum to prevent accidental bitwise or'ing with
 *       masks, as this enum represents groups of masks - it is not a mask itself.
 */
class VerboGroup
{
public:
   /// @brief List of verbosity flags.
   enum class Flag_T : unsigned
   {
      Global,
      Console,
      UnitTest,
      Warning,
      Error,

      ArgParser,
      DataLogger,
      FileIO,
      Logger,
      TextLogger,
      ThreadSafeProxy,
      MemIO,
      Ops,
      Rng,
      Timer,
      VerboGroup,
      YmAssert,
      YmDefs,
      YmUtils,

      Errstream // must be last
   };

   void set  (Flag_T const F) noexcept;
   void clear(Flag_T const F) noexcept;
   bool test (Flag_T const F) const noexcept;

private:
   std::array<
      std::atomic<unsigned>, // type
      static_cast<unsigned>( // size
         std::ceil( // exclude Errstream intentional - special handling required
            std::to_underlying(Flag_T::Errstream) / ym_getNBits<unsigned>()))
   > _flags{};
};

/// @brief Convenience alias (no _T suffix because of common usage).
using VF = VerboGroup::Flag_T;

} // ym
