/**
 * @file    objectgroups.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <utility>

/* TODO
 *
 * printf(ymL_Logger | ymL_Ymcept_Basic, ...)
 *
 * ymL_Logger expands to OGM_T::Logger_Basic | OGM_T::Logger_Detail | ... | OGM_T::Ymcept_Basic
 */

namespace ym
{

/// @brief Object Group Base type.
using OGB_T = uint32;

/** ObjectGroup_T
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
   enum class T : OGB_T
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

/// @brief Type alias for convenience.
using OG_T = ObjectGroup::T;

/** ObjectGroupMask_T
 *
 * @brief Bottom-level group of registered objects.
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
 */
struct ObjectGroupMask
{
   static_assert(ObjectGroup::getNGroups() <= (1_u32 << 24_u32),
                 "Underlying type cannot support # of desired groups");

#define YM_OG_FMT_MASK(Group_, Mask_) ((std::to_underlying(OG_T::Group_) << 8_u32) | Mask_)

   /// @brief Object group mask definitions.
   enum : OGB_T
   {
      Logger_Basic     = YM_OG_FMT_MASK(Logger,    0b0000'0001u),

      Ymception_Assert = YM_OG_FMT_MASK(Ymception, 0b0000'0001u)
   };

#undef YM_OG_FMT_MASK
};

/// @brief Type alias for convenience.
using OGM_T = ObjectGroupMask;

} // ym
