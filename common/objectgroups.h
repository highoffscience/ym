/**
 * @file    objectgroups.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <utility>

namespace ym
{

/** ObjectGroup
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
struct ObjectGroup
{
   /// @brief Object group definitions
   enum class T : uint32
   {
      Logger,
      Ymception,

      NGroups
   };

   /** getNGroups
    *
    * @brief Convenience method to get the # of object groups.
    *
    * @return OGB_T -- # of object groups defined.
    */
   static constexpr auto getNGroups(void) { return std::to_underlying(T::NGroups); }
};

/** OGMask
 *
 * @brief Object Group Mask. Bottom-level group of registered objects.
 *
 * @note We can have a maximum of 2^24 groups, and each group can contain a maximum
 *       of 8 flags.
 *
 * @note These masks are contained in a struct to scope them. We do not want a scoped
 *       enum because that prevents us doing math on them easily, like bitwise
 *       or'ing. The containing structure is a struct not a namespace so we can
 *       type alias it.
 *
 * @note                      x = group
 *                            y = mask
 *       -------------------------------------------------
 *       | xxxx'xxxx | xxxx'xxxx | xxxx'xxxx | yyyy'yyyy |
 *       -------------------------------------------------
 *
 * @note Example use:
 *
 *       void set(OGMask::T const Mask);           set(OGMask::Logger | OGMask::Ymception);
 */
 TODO // above example use shows this api is easy to misuse!
struct OGMask // Object Group Mask
{
   /// @brief Base (underlying) type of an object group mask.
   using T = std::underlying_type_t<ObjectGroup::T>;

   static_assert(ObjectGroup::getNGroups() <= (1_u32 << 24_u32),
                 "Underlying type cannot support # of desired groups");

#define YM_OG_FMT_MSK(Group_, Mask_) ((std::to_underlying(ObjectGroup::T::Group_) << 8_u32) | Mask_##_u32)
#define YM_OG_FMT_GRP(Group_       ) YM_OG_FMT_MSK(Group_, 0xff)

   /// @brief Object group mask definitions.
   enum : T
   {
      Logger           = YM_OG_FMT_GRP(Logger                ),
      Logger_Basic     = YM_OG_FMT_MSK(Logger,    0b0000'0001),
      Logger_Detail    = YM_OG_FMT_MSK(Logger,    0b0000'0010),

      Ymception        = YM_OG_FMT_GRP(Ymception             ),
      Ymception_Assert = YM_OG_FMT_MSK(Ymception, 0b0000'0001),
   };

#undef YM_OG_FMT_MSK
#undef YM_OG_FMT_GRP
};

} // ym
