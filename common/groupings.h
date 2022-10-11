/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

namespace ym
{

/**
 * Keep in alphabetical order
 */
enum class Grouping_T : uint32
{
   Logger,
   Ymception,

   NGroups
};

/**
 *
 */
enum class GroupingsMask_T : uint32
{
#define YM_GROUPING_FMT_MASK(Group, Mask) ((static_cast<uint32>(Grouping_T::Group) << 8u) | Mask)

   Logger_Basic     = YM_GROUPING_FMT_MASK(Logger,    0b0000'0001u),

   Ymception_Assert = YM_GROUPING_FMT_MASK(Ymception, 0b0000'0001u)

#undef YM_GROUPING_FMT_MASK
};

using GMask_T = GroupingsMask_T;

} // ym
