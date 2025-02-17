/**
 * @file    ymdefs.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 *
 * @note This file should be included in every file of the project. It provides
 *       standard declarations to be shared throughout.
 * 
 * @note Macros are prefixed "YM_".
 * @note Macros used for a particular purpose are prefixed "YM_SPECIAL_".
 * @note Macros used as helper functions are prefixed "YM_HELPER_".
 * @note Macros that are defined/not defined are suffixed "_DEFINED".
 * @note Macros that use #if semantics should have definitive values, ie
 *       #define YM_MY_FLAG 0/1 -> #if (YM_MY_FLAG)
 *          or should otherwise be
 *       #define YM_MY_FLAG_DEFINED -> #if defined(YM_MY_FLAG_DEFINED)
 *          otherwise the following would be a bug
 *       #define YM_MY_FLAG 0/1 -> #if defined(YM_MY_FLAG)
 *       #define YM_MY_FLAG     -> #if (YM_MY_FLAG) // macro may not be defined
 */

#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

// ----------------------------------------------------------------------------

/**
 * @brief Debug flag.
 */
#if !defined(YM_DBG)
   #define YM_DBG true
#endif

// ----------------------------------------------------------------------------

/**
 * @brief Helper define's for the current cpp standard.
 */
#if (__cplusplus >= 201703L)
   #if (__cplusplus >= 202002L)
      #if (__cplusplus >= 202302L)
         #define YM_CPP_STANDARD 23
      #else
         #define YM_CPP_STANDARD 20
      #endif
   #else
      #define YM_CPP_STANDARD 17
   #endif
#else
   #error "At least C++17 standard required"
#endif

// ----------------------------------------------------------------------------

/**
 * @brief Helper macros for compiler detection.
 *
 * @note CLANG, GNUC, and MSVC are the only compilers tested.
 */

#if defined(__clang__) // must be before GNU test because clang also defines __GNUG__
   #define YM_CLANG_COMPILER_DEFINED
#elif defined(__GNUG__)
   #define YM_GNU_COMPILER_DEFINED
#elif defined(_MSC_VER)
   #define YM_MSVC_COMPILER_DEFINED
#else
   #warning "Unknown compiler detected"
#endif

// ----------------------------------------------------------------------------

#if defined(YM_MSVC_COMPILER_DEFINED)

   /**
    * @brief MSVC shenanigans.
    *
    * @ref <https://docs.microsoft.com/en-us/cpp/preprocessor/warning>.
    */

   #pragma warning(disable: 26812) // stop bugging me about unscoped enums
   #pragma warning(error:    4062) // switch on all enum values
   #pragma warning(error:    4227) // reference of const should be pointer to const

#endif

// ----------------------------------------------------------------------------

/*
 * Custom global defines and macros.
 */

/**
 * @brief Helper macros for the "the big five".
 *
 * @ref <https://en.cppreference.com/w/cpp/language/rule_of_three>.
 *
 * @param ClassName_ -- Name of class.
 */

#define YM_NO_DEFAULT(     ClassName_ ) ClassName_              (void              ) = delete;
#define YM_NO_COPY(        ClassName_ ) ClassName_              (ClassName_ const &) = delete;
#define YM_NO_ASSIGN(      ClassName_ ) ClassName_ & operator = (ClassName_ const &) = delete;
#define YM_NO_MOVE_COPY(   ClassName_ ) ClassName_              (ClassName_ &&     ) = delete;
#define YM_NO_MOVE_ASSIGN( ClassName_ ) ClassName_ & operator = (ClassName_ &&     ) = delete;

/** YM_MACRO_OVERLOAD
 * 
 * @brief Helper macro to allow for macro overloading based on number of arguments.
 * 
 * @ref <https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments>
 * 
 * @note Example:
 *    #define YM_MY_MACRO(...) YM_MACRO_OVERLOAD(YM_MY_MACRO, __VA_ARGS__)
 *    #define YM_MY_MACRO1(First) ...
 *    #define YM_MY_MACRO2(First, Second) ...
 * 
 * @param MACRO_ -- Name of macro to overload.
 * @param ...    -- Args to pass to macro.
 */

// get number of arguments with __NARG__
#define YM_HELPER__NARG__(...)  YM_HELPER__NARG_I_(__VA_ARGS__, YM_HELPER__RSEQ_N())
#define YM_HELPER__NARG_I_(...) YM_HELPER__ARG_N(__VA_ARGS__)
#define YM_HELPER__ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define YM_HELPER__RSEQ_N() 8,7,6,5,4,3,2,1,0

// general definition for any function name
#define YM_HELPER_1_MACRO_OVERLOAD(Name_, N_) Name_##N_
#define YM_HELPER_2_MACRO_OVERLOAD(Name_, N_) YMHELPER_1_MACRO_OVERLOAD(Name_, N_)
#define YM_MACRO_OVERLOAD(MACRO_, ...) \
   YM_HELPER_2_MACRO_OVERLOAD(MACRO_, YM_HELPER__NARG__(__VA_ARGS__)) (__VA_ARGS__)

/** implicit
 * 
 * @brief Used to make constructors explicitly implicit.
 * 
 * @note Name is not "YM_IMPLICIT" because it is more natural to write "implicit".
 */
#if defined(implicit)
   // if we get here consider using YM_IMPLICIT (or ymimplicit)
   #error "implicit macro already defined"
#else
   #define implicit
#endif

// ----------------------------------------------------------------------------

namespace ym
{

/**
 * @brief Primitive typedefs.
 * 
 * @note The static_asserts for the higher precision floating point defines
 *       are structured unorthodoxically so all supported compilers can parse it.
 */

using rawstr = char const * ;
using uchar  = unsigned char;

using int8   = std::int8_t ; static_assert(sizeof(int8 ) == 1u, "int8  not of expected size");
using int16  = std::int16_t; static_assert(sizeof(int16) == 2u, "int16 not of expected size");
using int32  = std::int32_t; static_assert(sizeof(int32) == 4u, "int32 not of expected size");
using int64  = std::int64_t; static_assert(sizeof(int64) == 8u, "int64 not of expected size");
using int128 =
   #if defined(YM_GNU_COMPILER_DEFINED) || defined(YM_CLANG_COMPILER_DEFINED)
      __int128_t
   #else
      void
   #endif
   ; static_assert(!std::is_void<int128>::value || sizeof(int128) == 16u, "int128 not of expected size");

using uint8   = std::uint8_t ; static_assert(sizeof(uint8 ) == 1u, "uint8  not of expected size");
using uint16  = std::uint16_t; static_assert(sizeof(uint16) == 2u, "uint16 not of expected size");
using uint32  = std::uint32_t; static_assert(sizeof(uint32) == 4u, "uint32 not of expected size");
using uint64  = std::uint64_t; static_assert(sizeof(uint64) == 8u, "uint64 not of expected size");
using uint128 =
   #if defined(YM_GNU_COMPILER_DEFINED) || defined(YM_CLANG_COMPILER_DEFINED)
      __uint128_t
   #else
      void
   #endif
   ; static_assert(!std::is_void<uint128>::value || sizeof(uint128) == 16u, "uint128 not of expected size");

using float32  = float      ; static_assert(std::numeric_limits<float32 >::digits == 24, "float32  (mantissa) not of expected size");
using float64  = double     ; static_assert(std::numeric_limits<float64 >::digits == 53, "float64  (mantissa) not of expected size");
using floatext = long double; static_assert(std::numeric_limits<floatext>::digits >= 53, "floatext (mantissa) not of expected size");

/// @brief Convenience alias.
using uintptr = std::uintptr_t;
static_assert(std::is_same<uintptr, uint64>::value, "Unexpected uintptr type");

/// @brief Convenience alias.
using intptr = std::ptrdiff_t;
static_assert(std::is_same<intptr, int64        >::value, "Unexpected intptr type");
static_assert(std::is_same<intptr, std::intptr_t>::value, "Unexpected intptr type");

// ----------------------------------------------------------------------------

/** YM_HELPER_LITERAL_DECL
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
#define YM_HELPER_LITERAL_DECL(UDL_, TypeToCastTo_)                                                                       \
   constexpr inline auto operator"" _##UDL_(unsigned long long int    u) { return static_cast<TypeToCastTo_>(u); } \
   constexpr inline auto operator"" _##UDL_(              long double d) { return static_cast<TypeToCastTo_>(d); } \
                                                                                                                   \
   static_assert(std::is_same<decltype(  0_##UDL_), TypeToCastTo_>::value &&                                       \
                 std::is_same<decltype(0.0_##UDL_), TypeToCastTo_>::value,                                         \
                 "User defined literal "#UDL_" failed to cast");

YM_HELPER_LITERAL_DECL(i8,   int8    )
YM_HELPER_LITERAL_DECL(i16,  int16   )
YM_HELPER_LITERAL_DECL(i32,  int32   )
YM_HELPER_LITERAL_DECL(i64,  int64   )

YM_HELPER_LITERAL_DECL(u8,   uint8   )
YM_HELPER_LITERAL_DECL(u16,  uint16  )
YM_HELPER_LITERAL_DECL(u32,  uint32  )
YM_HELPER_LITERAL_DECL(u64,  uint64  )

YM_HELPER_LITERAL_DECL(f32,  float32 )
YM_HELPER_LITERAL_DECL(f64,  float64 )
YM_HELPER_LITERAL_DECL(fext, floatext)
// you're on your own initializing float128

} // ym
