/**
 * @file    verbogroup.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include <utility>

namespace ym
{

/** VerboGroup
 *
 * @brief Top-level group of registered objects (verbosity group).
 *
 * @note This grouping is a two-tiered mechanism to organize, to a minimal extent,
 *       groups of related functionality. The Grouping_T is used for the higher
 *       level groups and GroupingsMask_T is used for the finer groups within
 *       those higher level groups. They occupy different enums to make switching
 *       groups of bits, not just individual bits, easier to implement.
 *
 * @note This is a wrapped scoped enum to prevent accidental bitwise or'ing with
 *       masks, as this enum represents groups of masks - it is not a mask itself.
 *
 * @note Keep in alphabetical order.
 */
struct VerboGroup
{
   /// @brief Verbosity groups.
   enum class T : uint32
   {
      Debug,
      Error,
      General,

      ArgParser,       UnitTest_ArgParser,
      DataLogger,      UnitTest_DataLogger,
      FileIO,          UnitTest_FileIO,
      TextLogger,      UnitTest_TextLogger,
      ThreadSafeProxy, UnitTest_ThreadSafeProxy,
      MemIO,           UnitTest_MemIO,
      Ops,             UnitTest_Ops,
      Rng,             UnitTest_Rng,
      Timer,           UnitTest_Timer,
      Ymception,       UnitTest_Ymception,
      
      NGroups
   };

   /** getNGroups
    *
    * @brief Convenience method to get the # of verbosity groups.
    *
    * @returns auto -- # of verbosity groups defined.
    */
   static constexpr auto getNGroups(void) { return std::to_underlying(T::NGroups); }
};

static_assert(VerboGroup::getNGroups() <= (1_u32 << 24_u32),
              "Underlying type cannot support # of desired groups");

/** VerboGroupMask
 *
 * @brief Verbosity Group Mask definitions. Sub-groups represented as masks.
 *
 * @note We can have a maximum of 2^24 groups, and each group can contain a maximum
 *       of 8 flags.
 *
 * @note These masks are a scoped enum so the user cannot accidentally bitwise or
 *       them.
 *
 * @note                      x = group
 *                            y = mask
 *       -------------------------------------------------
 *       | xxxx'xxxx | xxxx'xxxx | xxxx'xxxx | yyyy'yyyy |
 *       -------------------------------------------------
 *
 * @note Example use:
 *
 *       void set(VGMask const Mask);       set(VGMask::Logger_Basic);
 */
struct VerboGroupMask
{
   /**
    * @brief Underlying mask definitions.
    */
   enum class T : std::underlying_type_t<VerboGroup::T>
   {

   /// @brief Macro to facilitate macro overloading.
   #define YM_FMT_MSK(...) YM_MACRO_OVERLOAD(YM_FMT_MSK, __VA_ARGS__)

   /// @brief Convenience macros.
   #define YM_FMT_MSK2(Group_, Mask_) ((std::to_underlying(VerboGroup::T::Group_) << 8_u32) | Mask_##_u32)
   #define YM_FMT_MSK1(Group_       ) YM_FMT_MSK2(Group_, 0xff)

   #define YM_MAKE_MSK_AND_UNIT_MSK(Name_)            Name_ = YM_FMT_MSK(           Name_), \
                                           UnitTest_##Name_ = YM_FMT_MSK(UnitTest_##Name_)

      Debug   = YM_FMT_MSK(Debug  ),
      Error   = YM_FMT_MSK(Error  ),
      General = YM_FMT_MSK(General),

      YM_MAKE_MSK_AND_UNIT_MSK(ArgParser      ),
      YM_MAKE_MSK_AND_UNIT_MSK(DataLogger     ),
      YM_MAKE_MSK_AND_UNIT_MSK(FileIO         ),
      YM_MAKE_MSK_AND_UNIT_MSK(TextLogger     ),
         TextLogger_Basic  = YM_FMT_MSK(TextLogger, 0b0000'0001),
         TextLogger_Detail = YM_FMT_MSK(TextLogger, 0b0000'0010),
      YM_MAKE_MSK_AND_UNIT_MSK(ThreadSafeProxy),
      YM_MAKE_MSK_AND_UNIT_MSK(MemIO          ),
      YM_MAKE_MSK_AND_UNIT_MSK(Ops            ),
      YM_MAKE_MSK_AND_UNIT_MSK(Rng            ),
         Rng_Prng = YM_FMT_MSK(Rng, 0b0000'0001),
         Rng_Trng = YM_FMT_MSK(Rng, 0b0000'0010),
      YM_MAKE_MSK_AND_UNIT_MSK(Timer          ),
      YM_MAKE_MSK_AND_UNIT_MSK(Ymception      ),
         Ymception_Assert = YM_FMT_MSK(Ymception, 0b0000'0001),

   // don't pollute namespace
   #undef YM_MAKE_MSK_AND_UNIT_MSK
   #undef YM_FMT_MSK1
   #undef YM_FMT_MSK2
   #undef YM_FMT_MSK
   };

   /**
    * @brief Convenience functions to grab the group or mask as the underlying type.
    * 
    * @param VG -- The verbosity group mask
    * 
    * @returns auto -- Desired underlying type
    */
   static constexpr auto getGroup     (T const VG) { return std::to_underlying(VG) >> 8_u32;    }
   static constexpr auto getMask      (T const VG) { return std::to_underlying(VG) &  0xff_u32; }
   static constexpr auto getMaskAsByte(T const VG) { return static_cast<uint8>(VG);             }
};

/// @brief Convenience alias (no _T suffix because of common usage).
using VG = VerboGroupMask::T;

} // ym
