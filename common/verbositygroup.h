/**
 * @file    verbositygroup.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <utility>

namespace ym
{

/** VerbosityGroup
 *
 * @brief Top-level group of registered objects.
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
struct VerbosityGroup
{
   /// @brief Verbosity groups.
   enum class T : uint32
   {
      Logger,
      Ymception,

      NGroups
   };

   /** getNGroups
    *
    * @brief Convenience method to get the # of verbosity groups.
    *
    * @return auto -- # of verbosity groups defined.
    */
   static constexpr auto getNGroups(void) { return std::to_underlying(T::NGroups); }
};

static_assert(VerbosityGroup::getNGroups() <= (1_u32 << 24_u32),
              "Underlying type cannot support # of desired groups");

/** VGMask_T
 *
 * @brief Verbosity Group Mask. Sub-groups represented as masks.
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

struct TODOVG
{

/// @brief Verbosity group mask definitions.
enum class VGMask_T : std::underlying_type_t<VerbosityGroup::T>
{

/// @brief Convenience macros
#define YM_OG_FMT_MSK(Group_, Mask_) ((std::to_underlying(VerbosityGroup::T::Group_) << 8_u32) | Mask_##_u32)
#define YM_OG_FMT_GRP(Group_       ) YM_OG_FMT_MSK(Group_, 0xff)

   Logger           = YM_OG_FMT_GRP(Logger                ),
   Logger_Basic     = YM_OG_FMT_MSK(Logger,    0b0000'0001),
   Logger_Detail    = YM_OG_FMT_MSK(Logger,    0b0000'0010),

   Ymception        = YM_OG_FMT_GRP(Ymception             ),
   Ymception_Assert = YM_OG_FMT_MSK(Ymception, 0b0000'0001),

// don't pollute namespace
#undef YM_OG_FMT_GRP
#undef YM_OG_FMT_MSK
};

constexpr auto operator | ()

};

} // ym
