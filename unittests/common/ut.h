/**
 * @file    ut.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

// TODO rename to utdefs.h

// ----------------------------------------------------------------------------

#if defined(YM_UNITTEST)
static_assert(__cplusplus == 201703L, "C++17 standard required (for cppyy)");
#endif // YM_UNITTEST

// ----------------------------------------------------------------------------

namespace ym::utdefs
{

/**
 * @brief Primitive typedefs.
 */
using str     = char const * ;
using schar   =   signed char;
using uchar   = unsigned char;

using int8    = signed char    ; static_assert(sizeof(int8   ) ==  1ul, "int8  not 1 byte" );
using int16   = signed short   ; static_assert(sizeof(int16  ) ==  2ul, "int16 not 2 bytes");
using int32   = signed int     ; static_assert(sizeof(int32  ) ==  4ul, "int32 not 4 bytes");
using int64   = signed long    ; static_assert(sizeof(int64  ) ==  8ul, "int64 not 8 bytes");

using uint8   = unsigned char  ; static_assert(sizeof(uint8  ) ==  1ul, "uint8 not 1 byte" );
using uint16  = unsigned short ; static_assert(sizeof(uint16 ) ==  2ul, "uint8 not 2 bytes");
using uint32  = unsigned int   ; static_assert(sizeof(uint32 ) ==  4ul, "uint8 not 4 bytes");
using uint64  = unsigned long  ; static_assert(sizeof(uint64 ) ==  8ul, "uint8 not 8 bytes");

using float32 = float          ; static_assert(sizeof(float32) ==  4ul, "float32 not 4 bytes");
using float64 = double         ; static_assert(sizeof(float64) ==  8ul, "float64 not 8 bytes");
using float80 = long double    ; static_assert(sizeof(float80) >= 10ul, "float80 not at least 10 bytes");

} // ym::ut
