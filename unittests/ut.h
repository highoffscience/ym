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

namespace ut
{

/**
 * @brief Primitive typedefs.
 * 
 * @note Names are mangled to prevent clashes with defines in ym.h.
 */
using utstr     = char const *   ;
using utuchar   = unsigned char  ;

using utint8    = signed char    ; static_assert(sizeof(utint8   ) ==  1, "utint8  not 1 byte" );
using utint16   = signed short   ; static_assert(sizeof(utint16  ) ==  2, "utint16 not 2 bytes");
using utint32   = signed int     ; static_assert(sizeof(utint32  ) ==  4, "utint32 not 4 bytes");
using utint64   = signed long    ; static_assert(sizeof(utint64  ) ==  8, "utint64 not 8 bytes");

using utuint8   = unsigned char  ; static_assert(sizeof(utuint8  ) ==  1, "utuint8 not 1 byte" );
using utuint16  = unsigned short ; static_assert(sizeof(utuint16 ) ==  2, "utuint8 not 2 bytes");
using utuint32  = unsigned int   ; static_assert(sizeof(utuint32 ) ==  4, "utuint8 not 4 bytes");
using utuint64  = unsigned long  ; static_assert(sizeof(utuint64 ) ==  8, "utuint8 not 8 bytes");

using utfloat32 = float          ; static_assert(sizeof(utfloat32) ==  4, "utfloat32 not 4 bytes");
using utfloat64 = double         ; static_assert(sizeof(utfloat64) ==  8, "utfloat64 not 8 bytes");
using utfloat80 = long double    ; static_assert(sizeof(utfloat80) >= 10, "utfloat80 not at least 10 bytes");

} // ut
