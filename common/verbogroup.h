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
   /// @brief List of verbosity groups.
   enum class Groups_T : unsigned
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
      
      NGroups
   };

   YM_DECL_YMASSERT(Error)

   void set  (Groups_T const G);
   void clear(Groups_T const G);
   bool test (Groups_T const G) const;

private:
   std::array<
      std::atomic<unsigned>, // type
      static_cast<unsigned>(
         std::ceil(
            std::to_underlying(Groups_T::NGroups) / ymGetNBits<unsigned>())) // size
   > _flags{};
};

/// @brief Convenience alias (no _T suffix because of common usage).
using VG = VerboGroup::Groups_T;

} // ym
