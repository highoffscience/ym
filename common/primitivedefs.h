/**
 * @file    primitivedefs.h
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

#include <algorithm>
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
 * @note CLANG, GNUC, and MSVC are the only compilers tested.
 */

#if defined(__clang__) // must be before GNU test because clang also defines __GNUG__
   #define YM_CLANG_COMPILER
#elif defined(__GNUG__)
   #define YM_GNU_COMPILER
#elif defined(_MSC_VER)
   #define YM_MSVC_COMPILER
#else
   #warning "Unknown compiler detected"
#endif

// ----------------------------------------------------------------------------

#if defined(YM_MSVC_COMPILER)

   /**
    * @brief MSVC shenanigans.
    *
    * @ref <https://docs.microsoft.com/en-us/cpp/preprocessor/warning>.
    */

   #pragma warning(disable: 26812) // stop bugging me about unscoped enums
   #pragma warning(error:    4062) // switch on all enum values
   #pragma warning(error:    4227) // reference of const should be pointer to const

#endif // YM_MSVC_COMPILER

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
 * @brief Primitive typedefs.
 * 
 * @note The static_asserts for the higher precision floating point defines
 *       are structured unorthodoxically so all supported compilers can parse it.
 */

using rawstr = char const *  ;
using uchar  = unsigned char ;

using int8   = std::int8_t  ; static_assert(sizeof(int8 ) == 1u, "int8  not of expected size");
using int16  = std::int16_t ; static_assert(sizeof(int16) == 2u, "int16 not of expected size");
using int32  = std::int32_t ; static_assert(sizeof(int32) == 4u, "int32 not of expected size");
using int64  = std::int64_t ; static_assert(sizeof(int64) == 8u, "int64 not of expected size");
using int128 =
   #if defined(YM_GNU_COMPILER) || defined(YM_CLANG_COMPILER)
      __int128_t
   #else
      void
   #endif
   ; static_assert(!std::is_void_v<int128> || sizeof(int128) == 16ul, "int128 not of expected size");

using uint8   = std::uint8_t  ; static_assert(sizeof(uint8 ) == 1u, "uint8  not of expected size");
using uint16  = std::uint16_t ; static_assert(sizeof(uint16) == 2u, "uint16 not of expected size");
using uint32  = std::uint32_t ; static_assert(sizeof(uint32) == 4u, "uint32 not of expected size");
using uint64  = std::uint64_t ; static_assert(sizeof(uint64) == 8u, "uint64 not of expected size");
using uint128 =
   #if defined(YM_GNU_COMPILER) || defined(YM_CLANG_COMPILER)
      __uint128_t
   #else
      void
   #endif
   ; static_assert(!std::is_void_v<uint128> || sizeof(uint128) == 16ul, "uint128 not of expected size");

using float32  = float          ; static_assert(std::numeric_limits<float32 >::digits == 24, "float32  (mantissa) not of expected size");
using float64  = double         ; static_assert(std::numeric_limits<float64 >::digits == 53, "float64  (mantissa) not of expected size");
using floatext = long double    ; static_assert(std::numeric_limits<floatext>::digits >= 53, "floatext (mantissa) not of expected size");

/// @brief Convenience alias.
using uintptr = std::uintptr_t;
static_assert(std::is_same_v<uintptr, uint64>, "Unexpected uintptr type");

/** PtrToInt_T
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
union PtrToInt_T
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

/** StringLiteral
 * 
 * @brief Wrapper to trick compiler into accepting C-style strings as template arguments.
 * 
 * @tparam N -- # of characters (size of) of string literal.
 */
template <std::size_t N>
struct StringLiteral
{
   YM_IMPLICIT constexpr StringLiteral(const char (&str)[N])
   {
      std::copy_n(str, N, value);
   }
   
   char value[N];
};

// ----------------------------------------------------------------------------

/** ymCastPtrTo
 * 
 * @brief Casts given pointer to byte pointer.
 * 
 * @note According to @ref <https://en.cppreference.com/w/cpp/language/object>, any object can be
 *       inspected assuming an underlying representation of bytes.
 * 
 * @note A reinterpret_cast will not convert a pointer of arbitrary type to another, must cast
 *       to void first. We can avoid an explicit cast to void by just accepting a void * since
 *       pointers can be implicitely cast to void.
 * 
 * @tparam T -- Data type to cast to.
 * 
 * @param Data_Ptr -- Pointer to object(s).
 * 
 * @returns T const * -- Pointer to object(s) represented as an array of T.
 */
template <typename T>
constexpr auto const * ymCastPtrTo(void const * const Data_Ptr)
{
   return static_cast<T const *>(Data_Ptr);
}

/** ymCastPtrTo
 * 
 * @brief Non-const version of above.
 * 
 * @tparam T -- Data type to cast to.
 * 
 * @param data_Ptr -- Pointer to object(s).
 * 
 * @returns T * -- Pointer to object(s) represented as an array of T.
 */
template <typename T>
constexpr auto * ymCastPtrTo(void * const data_Ptr)
{
   return static_cast<T *>(data_Ptr);
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
   constexpr auto operator"" _##UDL_(unsigned long long int    u) { return static_cast<TypeToCastTo_>(u); } \
   constexpr auto operator"" _##UDL_(              long double d) { return static_cast<TypeToCastTo_>(d); } \
                                                                                                            \
   static_assert(std::is_same_v<decltype(  0_##UDL_), TypeToCastTo_> &&                                     \
                 std::is_same_v<decltype(0.0_##UDL_), TypeToCastTo_>,                                       \
                 "User defined literal "#UDL_" failed to cast");

YM_LITERAL_DECL(i8,   int8    )
YM_LITERAL_DECL(i16,  int16   )
YM_LITERAL_DECL(i32,  int32   )
YM_LITERAL_DECL(i64,  int64   )

YM_LITERAL_DECL(u8,   uint8   )
YM_LITERAL_DECL(u16,  uint16  )
YM_LITERAL_DECL(u32,  uint32  )
YM_LITERAL_DECL(u64,  uint64  )

YM_LITERAL_DECL(f32,  float32 )
YM_LITERAL_DECL(f64,  float64 )
YM_LITERAL_DECL(fext, floatext)
// you're on your own initializing float128

// don't pollute namespace
#undef YM_LITERAL_DECL

} // ym
