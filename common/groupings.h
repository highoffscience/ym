/**
 * @file    groupings.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

namespace ym
{

/**
 * This grouping is a two-tiered mechanism to organize, to a minimal extent,
 *  groups of related functionality. The Grouping_T is used for the higher
 *  level groups and GroupingsMask_T is used for the finer groups within
 *  those higher level groups. They occupy different enums to make switching
 *  groups of bits, not just individual bits, easier to implement.
 *
 * Keep in alphabetical order.
 */
enum Grouping_T : uint32
{
   Logger,
   Ymception,

   NGroups
};

// see below documentation for explanation on this check
static_assert(Grouping_T::NGroups < (1u << 24u),
              "Underlying type cannot support # of desired groups");

/**
 * We can have a maximum of 2^24 groups, and each group can contain a maximum
 *  of 8 flags.
 *
 * x = group
 * y = mask
 * -------------------------------------------------
 * | xxxx'xxxx | xxxx'xxxx | xxxx'xxxx | yyyy'yyyy |
 * -------------------------------------------------
 */
enum GroupingsMask_T : uint32
{
#define YM_GROUPING_FMT_MASK(Group, Mask) ((Grouping_T::Group << 8u) | Mask)

   Logger_Basic     = YM_GROUPING_FMT_MASK(Logger,    0b0000'0001u),

   Ymception_Assert = YM_GROUPING_FMT_MASK(Ymception, 0b0000'0001u)

#undef YM_GROUPING_FMT_MASK
};

// type alias for convenience
using GMask_T = GroupingsMask_T;

} // ym
