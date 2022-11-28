/**
 * @file    ym_ut.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#if defined(YM_UNITTEST)
static_assert(__cplusplus == 201703L, "C++17 standard required (for cppyy)");
#endif // YM_UNITTEST

namespace ym::ut
{

// /**
//  * @brief Primitive typedefs.
//  */

// using str     = char const *   ;
// using uchar   = unsigned char  ;

// using int8    = signed char    ; static_assert(sizeof(int8   ) == 1, "int8  not 1 byte" );
// using int16   = signed short   ; static_assert(sizeof(int16  ) == 2, "int16 not 2 bytes");
// using int32   = signed int     ; static_assert(sizeof(int32  ) == 4, "int32 not 4 bytes");
// using int64   = signed long    ; static_assert(sizeof(int64  ) == 8, "int64 not 8 bytes");

// using uint8   = unsigned char  ; static_assert(sizeof(uint8  ) == 1, "uint8 not 1 byte" );
// using uint16  = unsigned short ; static_assert(sizeof(uint16 ) == 2, "uint8 not 2 bytes");
// using uint32  = unsigned int   ; static_assert(sizeof(uint32 ) == 4, "uint8 not 4 bytes");
using ut_uint64  = unsigned long  ; static_assert(sizeof(ut_uint64 ) == 8, "ut_uint8 not 8 bytes");

using ut_float32 = float          ; static_assert(sizeof(ut_float32) == 4, "ut_float32 not 4 bytes");
using ut_float64 = double         ; static_assert(sizeof(ut_float64) == 8, "ut_float64 not 8 bytes");

} // ym::ut
