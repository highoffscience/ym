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
enum class VGGroup_T : uint32
{
   Logger,
   Ymception,

   NVGGroups
};

/**
 * 
 */
enum class VGMask_T : uint32
{
#define YM_VG_FMT_MASK(Group, Mask) ((static_cast<uint32>(VGGroup_T::Group) << 8u) | Mask)

   Logger_Basic = YM_VG_FMT_MASK(Logger, 0b0000'0001u),

   Ymception_Assert = YM_VG_FMT_MASK(Ymception, 0b0000'0001u),
   Ymception_Throw  = YM_VG_FMT_MASK(Ymception, 0b0000'0010u)

#undef YM_VG_FMT_MASK
};

} // ym
