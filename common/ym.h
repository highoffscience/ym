/**
 * @file    ym.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 * 
 * This file should be included in every file of the project. It provides
 *  standard declarations to be shared throughout.
 * 
 * _WIN32 refers to using the Windows API - not necessarily 32-bit Windows.
 */

#pragma once

#include <alloca.h>
#include <type_traits>

#if defined(_WIN32)

/**
 * @ref <https://docs.microsoft.com/en-us/cpp/preprocessor/warning>.
 */
#pragma warning(disable: 26812) // stop bugging me about unscoped enums
#pragma warning(error:    4062) // switch on all enum values
#pragma warning(error:    4227) // reference of const should be pointer to const

#if defined(_DEBUG)
#define YM_DBG
#endif // _DEBUG

#endif // _WIN32

#if !defined(YM_DBG)
#define YM_DBG
#endif // !YM_DBG

#if !defined(YM_EXPERIMENTAL)
#define YM_EXPERIMENTAL
#endif // !YM_EXPERIMENTAL

/**
 * Helper macros for the "the big five".
 * @ref <https://en.cppreference.com/w/cpp/language/rule_of_three>.
 */
#define YM_NO_DEFAULT(        ClassName_ ) ClassName_              (void              ) = delete;
#define YM_NO_COPY(           ClassName_ ) ClassName_              (ClassName_ const &) = delete;
#define YM_NO_ASSIGN(         ClassName_ ) ClassName_ & operator = (ClassName_ const &) = delete;
#define YM_NO_MOVE_CONSTRUCT( ClassName_ ) ClassName_              (ClassName_ &&     ) = delete;
#define YM_NO_MOVE_ASSIGN(    ClassName_ ) ClassName_ & operator = (ClassName_ &&     ) = delete;

namespace ym
{

using str     = char const * ;
using uchar   = unsigned char;

using int8    = signed char    ; static_assert(sizeof(int8   ) ==  1, "int8    not 1 byte" );
using int16   = signed short   ; static_assert(sizeof(int16  ) ==  2, "int16   not 2 bytes");
using int32   = signed int     ; static_assert(sizeof(int32  ) ==  4, "int32   not 4 bytes");
using int64   = signed long    ; static_assert(sizeof(int64  ) ==  8, "int64   not 8 bytes");

using uint8   = unsigned char  ; static_assert(sizeof(uint8  ) ==  1, "uint8   not 1 byte" );
using uint16  = unsigned short ; static_assert(sizeof(uint16 ) ==  2, "uint16  not 2 bytes");
using uint32  = unsigned int   ; static_assert(sizeof(uint32 ) ==  4, "uint32  not 4 bytes");
using uint64  = unsigned long  ; static_assert(sizeof(uint64 ) ==  8, "uint64  not 8 bytes");

using float32 = float          ; static_assert(sizeof(float32) ==  4, "float32 not 4 bytes");
using float64 = double         ; static_assert(sizeof(float64) ==  8, "float64 not 8 bytes");

using uintptr = uint64; static_assert(sizeof(uintptr) >= sizeof(void *), "uintptr cannot hold ptr value");

/** ymPtrToUint
 * 
 * @brief Casts non-member pointer to an appropriately sized uint.
 * 
 * @ref <https://en.cppreference.com/w/cpp/types/integer>.
 * 
 * @tparam T -- Pointer type
 * 
 * @param Ptr -- Pointer value
 * 
 * @return uintptr -- Ptr as an appropriately sized uint
 */
template <typename T>
requires( (std::is_pointer_v<T>        ) && // is non-member function pointer or data pointer
          (sizeof(uintptr) >= sizeof(T)) )  // is size small enough to store
constexpr auto ymPtrToUint(T const Ptr)
{
   return reinterpret_cast<uintptr>(Ptr);
}

#if defined(YM_EXPERIMENTAL)
#if !defined(_WIN32)

/**
 * @ref <https://en.cppreference.com/w/cpp/language/types>.
 */

using  int128  = __int128_t  ; static_assert(sizeof(int128 )  == 16, "int128  not 16 bytes");
using uint128  = __uint128_t ; static_assert(sizeof(uint128)  == 16, "uint128 not 16 bytes");

/*
 * Cpp Standard says float80 usually occupies 16 bytes.
 *
 * Type 'long double' not 16 bytes on all platforms - Cpp Standard only guarantees it is at least as large
 *  as type 'double', which is why we alias instead type '__float128'.
 */
using float80  = __float80  ; static_assert(sizeof(float80 ) >= 10, "float80  not at least 10 bytes");
using float128 = __float128 ; static_assert(sizeof(float128) == 16, "float128 not 16 bytes");

#endif //!_WIN32
#endif // YM_EXPERIMENTAL

/** ymStackAlloc
 * 
 * @brief Allocates requested amount of bytes on the stack at runtime.
 * 
 * @note Functionally moves the stack pointer to where you want. We mimic the
 *       behaviour of variable length arrays.
 * 
 * @note Memory allocated by this function automatically gets freed when the
 *       embedding function goes out of scope.
 * 
 * @ref <https://man7.org/linux/man-pages/man3/alloca.3.html>.
 * @ref <https://en.cppreference.com/w/c/language/array>.
 * 
 * @tparam T -- Type to allocate
 * 
 * @param NElements -- Number of T elements to allocate room for
 * 
 * @return T * -- Pointer to newly allocated stack memory
 */
template <typename T>
inline T * ymStackAlloc(uint32 const NElements)
{
   return static_cast<T *>(alloca(NElements * sizeof(T)));
}

} // ym
