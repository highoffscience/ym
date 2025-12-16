/**
 * @file    verbogroup.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymassert.h"
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
      Debug,
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

   #if (YM_UNITTEST)
      UnitTest_ArgParser,
      UnitTest_DataLogger,
      UnitTest_FileIO,
      UnitTest_Logger,
      UnitTest_TextLogger,
      UnitTest_ThreadSafeProxy,
      UnitTest_MemIO,
      UnitTest_Ops,
      UnitTest_Rng,
      UnitTest_Timer,
      UnitTest_VerboGroup,
      UnitTest_YmAssert,
      UnitTest_YmDefs,
      UnitTest_YmUtils,
   #endif
      
      NFlags
   };

   YM_DECL_YMASSERT(Error)

   void set  (Flag_T const F);
   void clear(Flag_T const F);
   bool test (Flag_T const F) const;

private:
   std::array<
      std::atomic<unsigned>, // type
      static_cast<unsigned>(
         std::ceil(
            std::to_underlying(Flag_T::NFlags) / ym_getNBits<unsigned>())) // size
   > _flags{};
};

/// @brief Convenience alias (no _T suffix because of common usage).
using VF = VerboGroup::Flag_T;

} // ym
