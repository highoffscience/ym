/**
 * @file    ut.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

// ----------------------------------------------------------------------------

#if defined(YM_UNITTEST)
static_assert(__cplusplus == 201703L, "C++17 standard required (for cppyy)");
#endif // YM_UNITTEST

// ----------------------------------------------------------------------------

namespace ym::ut
{

/**
 * @brief Primitive typedefs.
 * 
 * @note Names are mangled to prevent clashes with defines in ym.h.
 */
using utstr     = char const *   ;
using utuchar   = unsigned char  ;

using utint8    = signed char    ; static_assert(sizeof(utint8   ) ==  1ul, "utint8  not 1 byte" );
using utint16   = signed short   ; static_assert(sizeof(utint16  ) ==  2ul, "utint16 not 2 bytes");
using utint32   = signed int     ; static_assert(sizeof(utint32  ) ==  4ul, "utint32 not 4 bytes");
using utint64   = signed long    ; static_assert(sizeof(utint64  ) ==  8ul, "utint64 not 8 bytes");

using utuint8   = unsigned char  ; static_assert(sizeof(utuint8  ) ==  1ul, "utuint8 not 1 byte" );
using utuint16  = unsigned short ; static_assert(sizeof(utuint16 ) ==  2ul, "utuint8 not 2 bytes");
using utuint32  = unsigned int   ; static_assert(sizeof(utuint32 ) ==  4ul, "utuint8 not 4 bytes");
using utuint64  = unsigned long  ; static_assert(sizeof(utuint64 ) ==  8ul, "utuint8 not 8 bytes");

using utfloat32 = float          ; static_assert(sizeof(utfloat32) ==  4ul, "utfloat32 not 4 bytes");
using utfloat64 = double         ; static_assert(sizeof(utfloat64) ==  8ul, "utfloat64 not 8 bytes");
using utfloat80 = long double    ; static_assert(sizeof(utfloat80) >= 10ul, "utfloat80 not at least 10 bytes");

} // ym::ut
