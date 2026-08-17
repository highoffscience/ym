/**
 * @file    ymdefs.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 *
 * - This file should be included in every file of the project. It provides
 *   standard declarations to be shared throughout.
 *
 * - File used in unittests - maximum standard C++20.
 *
 * - Macros are prefixed "YM_".
 * - Macros used for a particular purpose are prefixed "YM_SPECIAL_".
 * - Macros used as helper functions are prefixed "YM_HELPER_".
 * - Macros that are defined/not defined are suffixed "_DEFINED".
 * - Macros that use `#if` semantics should have definitive values, ie
 *    - `#define` YM_MY_FLAG 0/1 -> `#if` (YM_MY_FLAG)
 *    -    or should otherwise be
 *    - `#define` YM_MY_FLAG_DEFINED -> `#if` defined(YM_MY_FLAG_DEFINED)
 *    -    otherwise the following would be a bug
 *    - `#define` YM_MY_FLAG 0/1 -> `#if` defined(YM_MY_FLAG)
 *    - `#define` YM_MY_FLAG     -> `#if` (YM_MY_FLAG) // macro may not be defined
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <climits>
#include <limits>
#include <type_traits>

// ----------------------------------------------------------------------------

/**
 * @brief Set to the current cpp standard.
 *
 * @internal YM_UNITTEST_ACTIVE_DEFINED is to compile this header *only* in objects with
 *           testsuites that include them.
 *           YM_UNITTEST is a source-wide define that edits properties that make inspecting
 *           code easier for clients easier.
 */
#if defined(__DOXYGEN__)
   #define YM_CPP_STANDARD CMAKE_CXX_STANDARD
#elif (__cplusplus >= 202302L)
   #if (__cplusplus > 202302L)
      #define YM_CPP_STANDARD 99
   #else
      #define YM_CPP_STANDARD 23
   #endif
#else
   #if defined(YM_UNITTEST_ACTIVE_DEFINED)
      #if (__cplusplus == 202002L)
         #define YM_CPP_STANDARD 20
      #else
         #error "C++20 standard required for cppyy based unittests"
      #endif
   #else
      #error "At least C++23 standard required"
   #endif
#endif

// ----------------------------------------------------------------------------

/*
 * @brief Helper macros for compiler detection.
 */
#if defined(_MSC_VER) // must be before CLANG test because clang-cl also defines _MSC_VER
   /// @brief Compiled with msvc compiler.
   #define YM_MSVC_COMPILER_DEFINED
#elif defined(__clang__) // must be before GNU test because clang also defines __GNUG__
   /// @brief Compiled with clang compiler.
   #define YM_CLANG_COMPILER_DEFINED
#elif defined(__GNUG__)
   /// @brief Compiled with gnu compiler.
   #define YM_GNU_COMPILER_DEFINED
#else
   #warning "Unknown compiler detected"
#endif

// ----------------------------------------------------------------------------

#if defined(YM_MSVC_COMPILER_DEFINED)
   /*
    * @brief MSVC shenanigans.
    *
    * - [Reference Guide](https://docs.microsoft.com/en-us/cpp/preprocessor/warning).
    */
   #pragma warning(disable: 26812) // stop bugging me about unscoped enums
   #pragma warning(error:    4062) // switch on all enum values
   #pragma warning(error:    4227) // reference of const should be pointer to const
#endif

/**
 * @brief Convenience method to print debug statements to console. Usually this macro is used
 *        to debug the global logger.
 *
 * @param Id_ -- Identifying tag.
 */
#define YM_DBG_PRINT(Id_) { std::fprintf(stdout, "DBG --<> "#Id_" <>--\n"); std::fflush(stdout); }

// ----------------------------------------------------------------------------

/*
 * Custom global defines and macros.
 */

/**
 * @name Big Five Declaration Deletion Macros.
 * @{
 *
 * @brief Helper macros for the "the big five".
 *
 * - [Reference Guide](https://en.cppreference.com/w/cpp/language/rule_of_three).
 *
 * @param ClassName_ -- Class name.
 *
 * __Unit Test__
 * - @ref ym::unit::TestSuite::BigFiveDeleteMacros.
 *
 * @test Shall delete the default constructor.
 * @test Shall delete the copy constructor.
 * @test Shall delete the assignment operator.
 * @test Shall delete the move constructor.
 * @test Shall delete the move operator.
 */
#define YM_NO_DEFAULT(     ClassName_ ) ClassName_              (void              ) = delete;
#define YM_NO_COPY(        ClassName_ ) ClassName_              (ClassName_ const &) = delete;
#define YM_NO_ASSIGN(      ClassName_ ) ClassName_ & operator = (ClassName_ const &) = delete;
#define YM_NO_MOVE_COPY(   ClassName_ ) ClassName_              (ClassName_ &&     ) = delete;
#define YM_NO_MOVE_ASSIGN( ClassName_ ) ClassName_ & operator = (ClassName_ &&     ) = delete;
/// @}

/*
 * Helper Param Expansion Macros.
 * Get number of arguments with __NARG__
 */
/// @cond INTERNAL
#define YM_HELPER__NARG__(...)  YM_HELPER__NARG_I_(__VA_ARGS__, YM_HELPER__RSEQ_N())
#define YM_HELPER__NARG_I_(...) YM_HELPER__ARG_N(__VA_ARGS__)
#define YM_HELPER__ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define YM_HELPER__RSEQ_N() 8,7,6,5,4,3,2,1,0

// general definition for any function name
#define YM_HELPER_1_MACRO_OVERLOAD(Name_, N_) Name_##N_
#define YM_HELPER_2_MACRO_OVERLOAD(Name_, N_) YM_HELPER_1_MACRO_OVERLOAD(Name_, N_)
/// @endcond

/**
 * @brief Helper macro to allow for macro overloading based on number of arguments.
 *
 * - [Reference](https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments).
 *
 * - Example:
 * @code{cpp}
 *    #define YM_MY_MACRO(...) YM_MACRO_OVERLOAD(YM_MY_MACRO, __VA_ARGS__)
 *    #define YM_MY_MACRO1(First) ...
 *    #define YM_MY_MACRO2(First, Second) ...
 * @endcode
 *
 * @param MACRO_ -- Name of macro to overload.
 * @param ...    -- Args to pass to macro.
 *
 * __Unit Test__
 * - @ref ym::unit::TestSuite::OverloadMacros.
 *
 * @test Shall allow custom macros to have overloads.
 */
#define YM_MACRO_OVERLOAD(MACRO_, ...) \
   YM_HELPER_2_MACRO_OVERLOAD(MACRO_, YM_HELPER__NARG__(__VA_ARGS__)) (__VA_ARGS__)

#if defined(implicit)
   // if we get here consider using YM_IMPLICIT (or ymimplicit)
   #error "implicit macro already defined"
#else
   /**
    * @brief Used to make constructors explicitly implicit.
    *
    * - Name is not "YM_IMPLICIT" because it is more natural to write "implicit".
    */
   #define implicit
#endif

/**
 * @brief Conditional noexcept specifier.
 *
 * - Mostly for use with functions with debug assertions, eg. @ref YMASSERTDBG.
 */
#if (YM_DEBUG)
   #define YMNOEXC
#else
   #define YMNOEXC noexcept
#endif

// ----------------------------------------------------------------------------

namespace ym
{

/**
 * @name Global Primitives & User Defined Literals.
 * @{
 *
 * @brief Typedefs for primitive data types.
 *
 * - The static_asserts for the higher precision floating point defines
 *   are structured unorthodoxically so all supported compilers can parse it.
 *
 * @internal Tests are defined in 2nd grouping below.
 */

using rawstr = char const *;
using uchar  = unsigned char;
using schar  =   signed char;

using int8   = std::int8_t ; static_assert(sizeof(int8 ) == 1uz, "int8  not of expected size");
using int16  = std::int16_t; static_assert(sizeof(int16) == 2uz, "int16 not of expected size");
using int32  = std::int32_t; static_assert(sizeof(int32) == 4uz, "int32 not of expected size");
using int64  = std::int64_t; static_assert(sizeof(int64) == 8uz, "int64 not of expected size");
using int128 =
   #if defined(YM_GNU_COMPILER_DEFINED) || defined(YM_CLANG_COMPILER_DEFINED)
      __int128_t
   #else
      void
   #endif
   ; static_assert(!std::is_void<int128>::value || sizeof(int128) == 16uz, "int128 not of expected size");

using uint8   = std::uint8_t ; static_assert(sizeof(uint8 ) == 1uz, "uint8  not of expected size");
using uint16  = std::uint16_t; static_assert(sizeof(uint16) == 2uz, "uint16 not of expected size");
using uint32  = std::uint32_t; static_assert(sizeof(uint32) == 4uz, "uint32 not of expected size");
using uint64  = std::uint64_t; static_assert(sizeof(uint64) == 8uz, "uint64 not of expected size");
using uint128 =
   #if defined(YM_GNU_COMPILER_DEFINED) || defined(YM_CLANG_COMPILER_DEFINED)
      __uint128_t
   #else
      void
   #endif
   ; static_assert(!std::is_void<uint128>::value || sizeof(uint128) == 16uz, "uint128 not of expected size");

using float32  = float      ; static_assert(std::numeric_limits<float32 >::digits == 24, "float32  (mantissa) not of expected size");
using float64  = double     ; static_assert(std::numeric_limits<float64 >::digits == 53, "float64  (mantissa) not of expected size");
using floatext = long double; static_assert(std::numeric_limits<floatext>::digits >= 53, "floatext (mantissa) not of expected size");

/// @}

/**
 * @brief Concept that describes a byte-like object.
 *
 * @tparam T -- Type to check against.
 */
template <typename T>
concept ByteLikeable =
   std::disjunction_v<
      std::is_same<std::remove_cv_t<T>, std::byte>,
      std::is_same<std::remove_cv_t<T>, char>,
      std::is_same<std::remove_cv_t<T>, schar>,
      std::is_same<std::remove_cv_t<T>, uchar>
   >;

/**
 * @brief Template for creating tag dispatch types.
 *
 * - [Reference Guide](https://www.fluentcpp.com/2018/04/27/tag-dispatching/).
 *
 * @param Name_ -- Name of type.
 */
#define YM_CREATE_TAG_DISPATCH_TYPE(Name_) struct Name_ { explicit constexpr Name_(void) noexcept = default; };

// ----------------------------------------------------------------------------

/**
 * @brief Returns number of bits in type T.
 *
 * @tparam T -- Type.
 *
 * @returns std::size_t -- The number of bits in type T.
 *
 * __Unit Test__
 * - @ref ym::unit::TestSuite::Test_getNBits.
 *
 * @test Shall return 16 when called with an i16 (2-byte) type.
 * @test Shall return 32 when called with an i32 (4-byte) type.
 */
template <typename T>
constexpr auto ym_getNBits(void) noexcept
{
   return sizeof(T) * static_cast<std::size_t>(CHAR_BIT);
}

/**
 * @brief Determines if the parameter is consider empty.
 *
 * - Can be overloaded for other types.
 *
 * @param S -- String to check.
 *
 * @returns True if empty, false otherwise.
 *
 * __Unit Test__
 * - @ref ym::unit::TestSuite::Func_empty.
 *
 * @test Shall return true when passed a nullptr.
 * @test Shall return true when passed an empty string.
 * @test Shall return false when passed the string "Torchic".
 */
constexpr auto ym_empty(rawstr const S) noexcept
{
   return
      ( S == nullptr) || // if null
      (*S == '\0'   );   // if empty
}

// ----------------------------------------------------------------------------

/**
 * @name Global Primitives & User Defined Literals.
 * @{
 *
 * @brief Defines a set of user-defined literals for commonly used types.
 *
 * - [Reference Guide](https://en.cppreference.com/w/cpp/language/user_literal).
 *
 * @param UDL_          -- Name of User Defined Literal.
 * @param TypeToCastTo_ -- Type to cast to.
 *
 * @returns auto -- Input casted to TypeToCastTo_.
 *
 * __Unit Test__
 * - @ref ym::unit::TestSuite::PrimitiveDefSuffixes.
 *
 * @test Shall define int8 user defined literal.
 * @test Shall define int16 user defined literal.
 * @test Shall define int32 user defined literal.
 * @test Shall define int64 user defined literal.
 *
 * @test Shall define uint8 user defined literal.
 * @test Shall define uint16 user defined literal.
 * @test Shall define uint32 user defined literal.
 * @test Shall define uint64 user defined literal.
 *
 * @test Shall define float32 user defined literal.
 * @test Shall define float64 user defined literal.
 * @test Shall define floatext user defined literal.
 */
#define YM_USER_LITERAL_DECL(UDL_, TypeToCastTo_)                                                               \
   constexpr inline auto operator""_##UDL_(unsigned long long int    u) { return static_cast<TypeToCastTo_>(u); } \
   constexpr inline auto operator""_##UDL_(              long double d) { return static_cast<TypeToCastTo_>(d); } \
                                                                                                                  \
   static_assert(std::is_same<decltype(  0_##UDL_), TypeToCastTo_>::value &&                                      \
                 std::is_same<decltype(0.0_##UDL_), TypeToCastTo_>::value,                                        \
                 "User defined literal "#UDL_" failed to cast");

YM_USER_LITERAL_DECL(i8,   int8 )
YM_USER_LITERAL_DECL(i16,  int16)
YM_USER_LITERAL_DECL(i32,  int32)
YM_USER_LITERAL_DECL(i64,  int64)

YM_USER_LITERAL_DECL(u8,   uint8 )
YM_USER_LITERAL_DECL(u16,  uint16)
YM_USER_LITERAL_DECL(u32,  uint32)
YM_USER_LITERAL_DECL(u64,  uint64)

YM_USER_LITERAL_DECL(f32,  float32 )
YM_USER_LITERAL_DECL(f64,  float64 )
YM_USER_LITERAL_DECL(fext, floatext)
// you're on your own initializing float128

/// @}

} // ym
