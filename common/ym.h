/**
 * @file    ym.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 *
 * @note This file should be included in every file of the project. It provides
 *       standard declarations to be shared throughout.
 */

#pragma once

#include <alloca.h>
#include <limits>
#include <type_traits>

#include <boost/predef.h>

// ----------------------------------------------------------------------------

/**
 * @brief Helper macros for compiler detection.
 *
 * @note We define our own macros because BOOST_COMP_XXX is always defined, just
 *       set to zero. Error prone since the programmer might forget to use the
 *       BOOST_COMP_XXX_AVAILABLE macro instead.
 * 
 * @note GNUC, CLANG, and MSVC are the only compilers tested.
 */

#if defined(BOOST_COMP_GNUC_AVAILABLE)
#define YM_IS_GNUC
#endif // BOOST_COMP_GNUC_AVAILABLE

#if defined(BOOST_COMP_CLANG_AVAILABLE)
#define YM_IS_CLANG
#endif // BOOST_COMP_CLANG_AVAILABLE

#if defined(BOOST_COMP_MSVC_AVAILABLE)
#define YM_IS_MSVC
#endif // BOOST_COMP_MSVC_AVAILABLE

// ----------------------------------------------------------------------------

#if defined(YM_IS_MSVC)

/**
 * @brief MSVC shenanigans.
 * 
 * @ref <https://docs.microsoft.com/en-us/cpp/preprocessor/warning>.
 */

#pragma warning(disable: 26812) // stop bugging me about unscoped enums
#pragma warning(error:    4062) // switch on all enum values
#pragma warning(error:    4227) // reference of const should be pointer to const

#endif // YM_IS_MSVC

// ----------------------------------------------------------------------------

/**
 * @brief Custom defines.
 */

#if defined(YM_IS_MSVC)
#if defined(_DEBUG)
#define YM_DBG
#endif // _DEBUG
#endif // YM_IS_MSVC

#if !defined(YM_DBG)
#define YM_DBG
#endif // !YM_DBG

#if !defined(YM_EXPERIMENTAL)
#define YM_EXPERIMENTAL
#endif // !YM_EXPERIMENTAL

// ----------------------------------------------------------------------------

namespace ym
{

/**
 * @brief Helper macros for the "the big five".
 *
 * @ref <https://en.cppreference.com/w/cpp/language/rule_of_three>.
 *
 * @param ClassName_ -- Name of class
 */

#define YM_NO_DEFAULT(        ClassName_ ) ClassName_              (void              ) = delete;
#define YM_NO_COPY(           ClassName_ ) ClassName_              (ClassName_ const &) = delete;
#define YM_NO_ASSIGN(         ClassName_ ) ClassName_ & operator = (ClassName_ const &) = delete;
#define YM_NO_MOVE_CONSTRUCT( ClassName_ ) ClassName_              (ClassName_ &&     ) = delete;
#define YM_NO_MOVE_ASSIGN(    ClassName_ ) ClassName_ & operator = (ClassName_ &&     ) = delete;

// ----------------------------------------------------------------------------

/**
 * @brief Primitive typedefs.
 */

#define YM_INT_INTEGRITY(Int_T_, ExpectedNUsefulBits_)                            \
   static_assert(std::numeric_limits<Int_T_>::is_signed, #Int_T_" not signed");   \
   static_assert(std::numeric_limits<Int_T_>::digits + 1 == ExpectedNUsefulBits_, \
      #Int_T_" doesn't have expected range of values");

#define YM_UNT_INTEGRITY(Unt_T_, ExpectedNUsefulBits_)                          \
   static_assert(!std::numeric_limits<Unt_T_>::is_signed, #Unt_T_" is signed"); \
   static_assert( std::numeric_limits<Unt_T_>::digits == ExpectedNUsefulBits_,  \
      #Unt_T_" doesn't have expected range of values");

#define YM_FLT_INTEGRITY(Flt_T_, ExpectedNMantissaBits_)                          \
   static_assert(std::is_floating_point_v<Flt_T_>, #Flt_T_" not floating point"); \
   static_assert(std::numeric_limits<Flt_T_>::digits == ExpectedNMantissaBits_,   \
      #Flt_T_" doesn't have expected range of values");

using str     = char const *   ;
using uchar   = unsigned char  ;

using int8    = signed char    ; YM_INT_INTEGRITY(int8    ,  8)
using int16   = signed short   ; YM_INT_INTEGRITY(int16   , 16)
using int32   = signed int     ; YM_INT_INTEGRITY(int32   , 32)
using int64   = signed long    ; YM_INT_INTEGRITY(int64   , 64)

using uint8   = unsigned char  ; YM_UNT_INTEGRITY(uint8   ,  8)
using uint16  = unsigned short ; YM_UNT_INTEGRITY(uint16  , 16)
using uint32  = unsigned int   ; YM_UNT_INTEGRITY(uint32  , 32)
using uint64  = unsigned long  ; YM_UNT_INTEGRITY(uint64  , 64)

using float32 = float          ; YM_FLT_INTEGRITY(float32 , 24)
using float64 = double         ; YM_FLT_INTEGRITY(float64 , 53)
using float80 = long double    ; YM_FLT_INTEGRITY(float80 , 64)

using uintptr = uint64; static_assert(sizeof(uintptr) >= sizeof(void *), "uintptr cannot hold ptr value");

/** ymPtrToUint
 *
 * @brief Casts non-member pointer to an appropriately sized uint.
 *
 * @ref <https://en.cppreference.com/w/cpp/types/integer>.
 *
 * @note It is important to make sure the size of the pointer is the exact size of the
 *       type we're trying to cast too. Too small or large will lead to undefined
 *       behaviour and subtle bugs.
 *
 * @note It is unrecommended to store function pointers as a pointer to void, and thus
 *       as a uint. However many compilers allow it because of the days of C, and hence
 *       a reinterpret_cast instead of a static_cast. As long as the type we are casting
 *       is the same size we will be ok. We explicitly disqualify member function
 *       pointers because they usually occupy 16 bytes. If necessary a convenience
 *       casting method similar to this one can be made and placed in the experimental
 *       block but there is no need and usually cleaner solutions exist.
 *
 * @tparam T -- Pointer type
 *
 * @param Ptr -- Pointer value
 *
 * @return uintptr -- Ptr as an appropriately sized uint
 */
template <typename T>
requires( (std::is_pointer_v<T>        ) && // is non-member function pointer or data pointer
          (sizeof(uintptr) == sizeof(T)) )  // is pointer size equal to uint size
constexpr auto ymPtrToUint(T const Ptr)
{
   return reinterpret_cast<uintptr>(Ptr);
}

/** YM_LITERAL_DECL
 *
 * @brief Defines a set of user-defined literals for commonly used types.
 *
 * @ref <https://en.cppreference.com/w/cpp/language/user_literal>
 *
 * @param UDL_          -- Name of User Defined Literal
 * @param TypeToCastTo_ -- Type to cast to
 *
 * @return auto -- Input casted to TypeToCastTo_
 */
#define YM_LITERAL_DECL(UDL_, TypeToCastTo_)                                                                \
   constexpr auto operator"" _##UDL_(unsigned long long int    i) { return static_cast<TypeToCastTo_>(i); } \
   constexpr auto operator"" _##UDL_(              long double d) { return static_cast<TypeToCastTo_>(d); } \
                                                                                                            \
   static_assert(std::is_same_v<decltype(  0_##UDL_), TypeToCastTo_> &&                                     \
                 std::is_same_v<decltype(0.0_##UDL_), TypeToCastTo_>,                                       \
                 "User defined literal "#UDL_" failed to cast");

YM_LITERAL_DECL(i8,  int8   )
YM_LITERAL_DECL(i16, int16  )
YM_LITERAL_DECL(i32, int32  )
YM_LITERAL_DECL(i64, int64  )

YM_LITERAL_DECL(u8,  uint8  )
YM_LITERAL_DECL(u16, uint16 )
YM_LITERAL_DECL(u32, uint32 )
YM_LITERAL_DECL(u64, uint64 )

YM_LITERAL_DECL(f32, float32)
YM_LITERAL_DECL(f64, float64)

#undef YM_LITERAL_DECL

/*
 * Experimental defines.
 */

#if defined(YM_EXPERIMENTAL)
#if !defined(_WIN32)

/**
 * @ref <https://en.cppreference.com/w/cpp/language/types>.
 */

// using  int128  = __int128_t  ; static_assert(sizeof(int128 )  == 16ul, "int128  not 16 bytes");
// using uint128  = __uint128_t ; static_assert(sizeof(uint128)  == 16ul, "uint128 not 16 bytes");

/*
 * Cpp Standard says float80 usually occupies 16 bytes.
 *
 * Type 'long double' not 16 bytes on all platforms - Cpp Standard only guarantees it is at least as large
 *  as type 'double', which is why we alias instead type '__float128'.
 */
// using float80  = __float80  ; static_assert(sizeof(float80 ) >= 10ul, "float80  not at least 10 bytes");
// using float128 = __float128 ; static_assert(sizeof(float128) == 16ul, "float128 not 16 bytes");

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
 * @ref <https://en.cppreference.com/w/c/language/array>. See section on VLA's.
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
