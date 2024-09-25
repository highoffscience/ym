/**
 * @file    ops.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"
#include "ymerror.h"
#include "ymutils.h"

#include <string>
#include <type_traits>

namespace ym
{

/** Ops
 *
 * @brief Collection of useful operations.
 */
class Ops
{
public:
   YM_NO_DEFAULT(Ops)

   static char castToChar(str const S);

   template <typename T>
   requires (std::is_floating_point_v<T>)
   static T castTo(std::string const & S) = delete;

   template <typename T>
   requires (std::is_integral_v<T>)
   static T castTo(std::string const & S,
                   uint32      const   Base = 10_u32) = delete;

   YM_DECL_YMERROR(OpsError)
   YM_DECL_YMERROR(OpsError, OpsError_BadCast)
};

template <> int8     Ops::castTo<int8>    (std::string const & S, uint32 const Base);
template <> int16    Ops::castTo<int16>   (std::string const & S, uint32 const Base);
template <> int32    Ops::castTo<int32>   (std::string const & S, uint32 const Base);
template <> int64    Ops::castTo<int64>   (std::string const & S, uint32 const Base);
template <> uint8    Ops::castTo<uint8>   (std::string const & S, uint32 const Base);
template <> uint16   Ops::castTo<uint16>  (std::string const & S, uint32 const Base);
template <> uint32   Ops::castTo<uint32>  (std::string const & S, uint32 const Base);
template <> uint64   Ops::castTo<uint64>  (std::string const & S, uint32 const Base);
template <> float32  Ops::castTo<float32> (std::string const & S);
template <> float64  Ops::castTo<float64> (std::string const & S);
template <> floatext Ops::castTo<floatext>(std::string const & S);

} // ym
