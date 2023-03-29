/**
 * @file    verbogroup.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

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
      ArgParser,
      Debug,
      General,
      Logger,
      UnitTest_ArgParser,
      UnitTest_Ops,
      Ymception,

      NGroups
   };

   /** getNGroups
    *
    * @brief Convenience method to get the # of verbosity groups.
    *
    * @return auto -- # of verbosity groups defined.
    */
   static constexpr auto getNGroups(void) { return ymToUnderlying(T::NGroups); }
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
   /// @brief Convenience macros.
   #define YM_FMT_MSK(Group_, Mask_) ((ymToUnderlying(VerboGroup::T::Group_) << 8_u32) | Mask_##_u32)
   #define YM_FMT_GRP(Group_       ) YM_FMT_MSK(Group_, 0xff)

      ArgParser          = YM_FMT_GRP(ArgParser             ),

      Debug              = YM_FMT_GRP(Debug                 ),

      General            = YM_FMT_GRP(General               ),

      Logger             = YM_FMT_GRP(Logger                ),
      Logger_Basic       = YM_FMT_MSK(Logger,    0b0000'0001),
      Logger_Detail      = YM_FMT_MSK(Logger,    0b0000'0010),

      UnitTest_ArgParser = YM_FMT_GRP(UnitTest_ArgParser    ),
      UnitTest_Ops       = YM_FMT_GRP(UnitTest_Ops          ),

      Ymception          = YM_FMT_GRP(Ymception             ),
      Ymception_Assert   = YM_FMT_MSK(Ymception, 0b0000'0001),

   // don't pollute namespace
   #undef YM_FMT_GRP
   #undef YM_FMT_MSK
   };

   /**
    * @brief Convenience functions to grab the group or mask as the underlying type.
    * 
    * @param VG -- The verbosity group mask
    * 
    * @return auto -- Desired underlying type
    */
   static constexpr auto getGroup      (T const VG) { return ymToUnderlying    (VG) >> 8_u32;    }
   static constexpr auto getMask       (T const VG) { return ymToUnderlying    (VG) &  0xff_u32; }
   static constexpr auto getMask_asByte(T const VG) { return static_cast<uint8>(VG);             }
};

/// @brief Convenience alias (no _T suffix because of common usage).
using VG = VerboGroupMask::T;

} // ym
