/**
 * @file    ymdefs.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 *
 * @note This file should be included in every file of the project. It provides
 *       standard declarations to be shared throughout.
 * 
 * @note Macros start with "YM_".
 * 
 * @note Macros used for a particular purpose start with "YM_SPECIAL_".
 */

#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

static_assert(__cplusplus >= 202002L, "At least C++20 standard required");

// ----------------------------------------------------------------------------

/**
 * @brief Helper macros for compiler detection.
 *
 * @note GNUC, CLANG, and MSVC are the only compilers tested.
 *
 * @note We define YM_IS_XXX_ID to verify compiler mutual exclusion. We don't
 *       want to define YM_IS_GNUC to have a value since it is now technically
 *       defined no matter the value so errors like
 *
 *          #define YM_IS_XXX 0
 *          #if defined(YM_IS_XXX)
 *             // this code executes!
 *          #endif
 *
 *        will be avoided.
 */

#if defined(__clang__)
   #define YM_IS_CLANG
   #define YM_IS_CLANG_ID 1
#else
   #define YM_IS_CLANG_ID 0
#endif // __clang__

#if defined(__GNUG__)
   #if !defined(YM_IS_CLANG) // clang defines __GNUG__ too
      #define YM_IS_GNU
      #define YM_IS_GNU_ID 1
   #else
      #define YM_IS_GNU_ID 0
   #endif // !YM_IS_CLANG
#endif // __GNUG__

#if defined(_MSC_VER)
   #define YM_IS_MSVC
   #define YM_IS_MSVC_ID 1
#else
   #define YM_IS_MSVC_ID 0
#endif // _MSC_VER

// mutual exclusion test
static_assert(YM_IS_CLANG_ID +
              YM_IS_GNU_ID   +
              YM_IS_MSVC_ID  == 1, 
              "Multiple (or no) compilers detected");

// don't pollute namespace
#undef YM_IS_CLANG_ID
#undef YM_IS_GNU_ID
#undef YM_IS_MSVC_ID

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

/*
 * Custom global defines and macros.
 */

#if defined(YM_IS_MSVC)
   #if defined(_DEBUG)
      #if !defined(YM_UT_DBG)
         static_assert(false, "Clashing intentions of debug levels detected")
      #endif // !YM_UT_DBG
   #endif // _DEBUG
#endif // YM_IS_MSVC

#if !defined(YM_EXPERIMENTAL)
   // #define YM_EXPERIMENTAL
#endif // !YM_EXPERIMENTAL

/**
 * @brief Helper macros for the "the big five".
 *
 * @ref <https://en.cppreference.com/w/cpp/language/rule_of_three>.
 *
 * @param ClassName_ -- Name of class.
 */

#define YM_NO_DEFAULT(        ClassName_ ) ClassName_              (void              ) = delete;
#define YM_NO_COPY(           ClassName_ ) ClassName_              (ClassName_ const &) = delete;
#define YM_NO_ASSIGN(         ClassName_ ) ClassName_ & operator = (ClassName_ const &) = delete;
#define YM_NO_MOVE_CONSTRUCT( ClassName_ ) ClassName_              (ClassName_ &&     ) = delete;
#define YM_NO_MOVE_ASSIGN(    ClassName_ ) ClassName_ & operator = (ClassName_ &&     ) = delete;

/** YM_MACRO_OVERLOAD
 * 
 * @brief Helper macro to allow for macro overloading based on number of arguments.
 * 
 * @ref <https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments>
 * 
 * @param MACRO_ -- Name of macro to overload.
 * @param ...    -- Args to pass to macro.
 */

// get number of arguments with __NARG__
#define YM__NARG__(...)  YM__NARG_I_(__VA_ARGS__, YM__RSEQ_N())
#define YM__NARG_I_(...) YM__ARG_N(__VA_ARGS__)
#define YM__ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define YM__RSEQ_N() 8,7,6,5,4,3,2,1,0

// general definition for any function name
#define YM_MACRO_OVERLOAD_HELPER_1(Name_, N_) Name_##N_
#define YM_MACRO_OVERLOAD_HELPER_2(Name_, N_) YM_MACRO_OVERLOAD_HELPER_1(Name_, N_)
#define YM_MACRO_OVERLOAD(MACRO_, ...) \
   YM_MACRO_OVERLOAD_HELPER_2(MACRO_, YM__NARG__(__VA_ARGS__)) (__VA_ARGS__)

/** YM_IMPLICIT
 * 
 * @brief Used to make constructors explicitly implicit.
 */
#define YM_IMPLICIT

// ----------------------------------------------------------------------------

namespace ym
{

/**
 * @brief Helper macros to perform integrity/sanity checks on primitive typedefs.
 *
 * @note We use std::numeric_limits<>::digits to verify range instead of sizeof
 *       because type may include padding or reserved bits. For example, sizeof
 *       float80 is typically 16, and aligned as such, even though it is only a
 *       10 byte type. To avoid confusion on how big a type actually is and to
 *       uniquely express the types expected range, use std::numeric_limits<>::digits.
 *
 * @param Prim_T_        -- Data type to perform integrity checks on.
 * @param NMantissaBits_ -- Expected # of bits in mantissa.
 */

// int = signed integer
#define YM_INT_INTEGRITY(Prim_T_, NMantissaBits_)                        \
   static_assert(std::numeric_limits<Prim_T_>::is_signed,                \
                 #Prim_T_" not signed");                                 \
                                                                         \
   static_assert(std::numeric_limits<Prim_T_>::digits == NMantissaBits_, \
                 #Prim_T_" doesn't have expected range");

// unt = unsigned integer
#define YM_UNT_INTEGRITY(Prim_T_, NMantissaBits_)                        \
   static_assert(!std::numeric_limits<Prim_T_>::is_signed,               \
                 #Prim_T_" is signed");                                  \
                                                                         \
   static_assert(std::numeric_limits<Prim_T_>::digits == NMantissaBits_, \
                 #Prim_T_" doesn't have expected range");

// flt = float
#define YM_FLT_INTEGRITY(Prim_T_, NMantissaBits_)                        \
   static_assert(std::is_floating_point_v<Prim_T_>,                      \
                 #Prim_T_" not floating point");                         \
                                                                         \
   static_assert(std::numeric_limits<Prim_T_>::digits == NMantissaBits_, \
                 #Prim_T_" doesn't have expected range");

/**
 * @brief Primitive typedefs.
 * 
 * @note The static_asserts for the higher precision floating point defines
 *       are structured unorthodoxically so all supported compilers can parse it.
 */

using str     = char const *   ;
using uchar   = unsigned char  ;

using int8    = signed char    ; YM_INT_INTEGRITY(int8   ,  7)
using int16   = signed short   ; YM_INT_INTEGRITY(int16  , 15)
using int32   = signed int     ; YM_INT_INTEGRITY(int32  , 31)
using int64   = signed long    ; YM_INT_INTEGRITY(int64  , 63)

using uint8   = unsigned char  ; YM_UNT_INTEGRITY(uint8  ,  8)
using uint16  = unsigned short ; YM_UNT_INTEGRITY(uint16 , 16)
using uint32  = unsigned int   ; YM_UNT_INTEGRITY(uint32 , 32)
using uint64  = unsigned long  ; YM_UNT_INTEGRITY(uint64 , 64)

using float32 = float          ; YM_FLT_INTEGRITY(float32, 24)
using float64 = double         ; YM_FLT_INTEGRITY(float64, 53)

using float80 = std::conditional_t<std::numeric_limits<long double>::digits == 64,
                   long double,
                   std::conditional_t<std::numeric_limits<long double>::digits == 113,
                      void,
                   #if defined(YM_IS_GNU)
                      __float80
                   #else
                      void
                   #endif // YM_IS_GNU
                   >
                >;
   static_assert(sizeof(std::conditional_t<std::is_void_v<float80>,
                           uint8[10], // float80 is not defined (ok)
                           float80    // float80 is defined
                        >) >= 10ul,
                 "float80 doesn't have expected size (range)");

using float128 = std::conditional_t<std::numeric_limits<long double>::digits == 113,
                    long double,
                 #if defined(YM_IS_GNU) || defined(YM_IS_CLANG)
                    __float128
                 #else
                    void
                 #endif // YM_IS_GNU || YM_IS_CLANG
                 >;

   static_assert(sizeof(std::conditional_t<std::is_void_v<float128>,
                           uint8[16], // float128 is not defined (ok)
                           float128   // float128 is defined
                        >) >= 16ul,
                 "float80 doesn't have expected size (range)");

// don't pollute namespace
#undef YM_FLT_INTEGRITY
#undef YM_UNT_INTEGRITY
#undef YM_INT_INTEGRITY

/// @brief Convenience alias.
using uintptr = std::uintptr_t;
static_assert(std::is_same_v<uintptr, uint64>, "Unexpected uintptr type");

/** PtrToUint_T
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
 * @tparam T -- Pointer type.
 */
template <typename T>
requires(std::is_pointer_v<T *>)
union PtrToUint_T
{
   T       * ptr_val;
   T     * * ptr_ptr_val;
   uintptr * uint_ptr_val;
   uintptr   uint_val;
};

/** YM_ARRAY_SIZE
 * 
 * @brief Computes the size of a C-style array.
 * 
 * @note Evaluates number of elements at compile time.
 * 
 * @param Array_ -- C_style array.
 */
#define YM_ARRAY_SIZE(Array_) (sizeof(Array_) / sizeof(*Array_))

// ----------------------------------------------------------------------------

/** ymCastToBytes
 * 
 * @brief Casts given pointer to byte pointer.
 * 
 * @note According to @ref <https://en.cppreference.com/w/cpp/language/object>, any object can be
 *       inspected assuming an underlying representation of bytes.
 * 
 * @tparam T -- Type of pointer.
 * 
 * @param Data_Ptr -- Pointer to object(s).
 * 
 * @returns uint8 const * -- Pointer to object(s) represented as an array of bytes.
 */
template<typename T>
constexpr uint8 const * ymCastToBytes(T const * const Data_Ptr) noexcept
{
   // a simple reinterpret cast will not suffice
   return
      static_cast<uint8 const *>(
         static_cast<void const *>(
            Data_Ptr
         )
      );
}

/** ymIsStrNonEmpty
 * 
 * @brief Convenience method to detect if a c-style string is non-empty.
 * 
 * @param S -- String to test.
 * 
 * @returns bool -- True if string S is non-empty, false otherwise.
 */
constexpr bool ymIsStrNonEmpty(str const S)
{
   return S && *S;
}

/** ymIsStrEmpty
 * 
 * @brief Convenience method to detect if a c-style string is null or empty.
 * 
 * @param S -- String to test.
 * 
 * @returns bool -- True if string S is null or empty, false otherwise.
 */
constexpr bool ymIsStrEmpty(str const S)
{
   return !ymIsStrNonEmpty(S);
}

// ----------------------------------------------------------------------------

/** YM_LITERAL_DECL
 *
 * @brief Defines a set of user-defined literals for commonly used types.
 *
 * @ref <https://en.cppreference.com/w/cpp/language/user_literal>.
 *
 * @param UDL_          -- Name of User Defined Literal.
 * @param TypeToCastTo_ -- Type to cast to.
 * 
 * @returns auto -- Input casted to TypeToCastTo_.
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
YM_LITERAL_DECL(f80, float80)

// don't pollute namespace
#undef YM_LITERAL_DECL

} // ym
