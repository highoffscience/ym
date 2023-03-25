/**
 * @file    ops.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "ymception.h"

namespace ym
{

/**
 * TODO
 */
class Ops
{
public:
   YM_NO_DEFAULT(Ops)

   template <typename T>
   static T castTo(str    const S,
                   uint32 const Base = 10_u32) = delete;

   YM_DECL_YMCEPT(OpsError)

   YM_DECL_YMCEPT(OpsError,         OpsError_BadCast         )
   YM_DECL_YMCEPT(OpsError_BadCast, OpsError_BadCastToChar   )
   YM_DECL_YMCEPT(OpsError_BadCast, OpsError_BadCastToInt8   )
   YM_DECL_YMCEPT(OpsError_BadCast, OpsError_BadCastToInt16  )
   YM_DECL_YMCEPT(OpsError_BadCast, OpsError_BadCastToInt32  )
   YM_DECL_YMCEPT(OpsError_BadCast, OpsError_BadCastToInt64  )
   YM_DECL_YMCEPT(OpsError_BadCast, OpsError_BadCastToUInt8  )
   YM_DECL_YMCEPT(OpsError_BadCast, OpsError_BadCastToUInt16 )
   YM_DECL_YMCEPT(OpsError_BadCast, OpsError_BadCastToUInt32 )
   YM_DECL_YMCEPT(OpsError_BadCast, OpsError_BadCastToUInt64 )
   YM_DECL_YMCEPT(OpsError_BadCast, OpsError_BadCastToFlt32  )
   YM_DECL_YMCEPT(OpsError_BadCast, OpsError_BadCastToFlt64  )
   YM_DECL_YMCEPT(OpsError_BadCast, OpsError_BadCastToFlt80  )
};

template <> char    Ops::castTo<char>   (str const S, uint32 const     );
template <> int8    Ops::castTo<int8>   (str const S, uint32 const Base);
template <> int16   Ops::castTo<int16>  (str const S, uint32 const Base);
template <> int32   Ops::castTo<int32>  (str const S, uint32 const Base);
template <> int64   Ops::castTo<int64>  (str const S, uint32 const Base);
template <> uint8   Ops::castTo<uint8>  (str const S, uint32 const Base);
template <> uint16  Ops::castTo<uint16> (str const S, uint32 const Base);
template <> uint32  Ops::castTo<uint32> (str const S, uint32 const Base);
template <> uint64  Ops::castTo<uint64> (str const S, uint32 const Base);
template <> float32 Ops::castTo<float32>(str const S, uint32 const     );
template <> float64 Ops::castTo<float64>(str const S, uint32 const     );
template <> float80 Ops::castTo<float80>(str const S, uint32 const     );

} // ym
