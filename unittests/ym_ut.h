/**
 * @file    ym_ut.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

// ----------------------------------------------------------------------------

static_assert(__cplusplus == 201703L, "C++17 standard required (for cppyy)");

// ----------------------------------------------------------------------------

namespace ym::ut
{

/**
 * @brief Primitive typedefs.
 * 
 * @note Names are mangled to prevent clashes with defines in ym.h.
 */
using utStr     = char const *   ;
using utUchar   = unsigned char  ;

using utInt8    = signed char    ; static_assert(sizeof(utInt8   ) ==  1, "utInt8  not 1 byte" );
using utInt16   = signed short   ; static_assert(sizeof(utInt16  ) ==  2, "utInt16 not 2 bytes");
using utInt32   = signed int     ; static_assert(sizeof(utInt32  ) ==  4, "utInt32 not 4 bytes");
using utInt64   = signed long    ; static_assert(sizeof(utInt64  ) ==  8, "utInt64 not 8 bytes");

using utUint8   = unsigned char  ; static_assert(sizeof(utUint8  ) ==  1, "utUint8 not 1 byte" );
using utUint16  = unsigned short ; static_assert(sizeof(utUint16 ) ==  2, "utUint8 not 2 bytes");
using utUint32  = unsigned int   ; static_assert(sizeof(utUint32 ) ==  4, "utUint8 not 4 bytes");
using utUint64  = unsigned long  ; static_assert(sizeof(utUint64 ) ==  8, "utUint8 not 8 bytes");

using utFloat32 = float          ; static_assert(sizeof(utFloat32) ==  4, "utFloat32 not 4 bytes");
using utFloat64 = double         ; static_assert(sizeof(utFloat64) ==  8, "utFloat64 not 8 bytes");
using utFloat80 = long double    ; static_assert(sizeof(utFloat80) >= 10, "utFloat80 not at least 10 bytes");

} // ym::ut
