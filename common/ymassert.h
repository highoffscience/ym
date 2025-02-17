/**
 * @file    ymassert.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "fmt/core.h"

#include <string>
#include <type_traits>
#include <utility>

#if (YM_YES_EXCEPTIONS)
   #include <exception>
#endif

#if (YM_YES_EXCEPTIONS)
   #define YM_DEFAULT_ASSERT_HANDLER throw
#else
   #define YM_DEFAULT_ASSERT_HANDLER ymassert_Base::defaultNoExceptHandler
#endif

/// @brief Convenience macro.
#define YM_DAH YM_DEFAULT_ASSERT_HANDLER

/** YMASSERT
 *
 * @brief Macro to assert on a condition.
 *
 * @param Cond_    -- Condition - true for happy path, false triggers the assert.
 * @param Derived_ -- Ymassert class to handle assert.
 * @param Handler_ -- Callback function if assert fails.
 * @param Format_  -- Format string.
 * @param ...      -- Arguments.
 */
 #define YMASSERT(                                             \
      Cond_,                                                   \
      Derived_,                                                \
      Handler_,                                                \
      Format_,                                                 \
      ...)                                                     \
   static_assert(std::is_convertible_v<decltype(Cond_), bool>, \
      "Condition must be convertible to bool");                \
   static_assert(std::is_base_of_v<ymassert_Base, Derived_>,   \
      "Assert class must be of type ymassert_Base");           \
   static_assert(std::is_same_v<Format_, rawstr>,              \
      "Format must be a string literal");                      \
   if (!(Condition_))                                          \
   {                                                           \
      Handler_(DerivedYmassert_(                               \
         "Assert @ \"{}:{}\": "Format_,                        \
         __FILE__,                                             \
         __LINE__,                                             \
         __VA_ARGS__));                                        \
   }

/** YM_DECL_YMASSERT
*
* @brief Declares a custom error class.
*
* @note See definitions of YM_DECL_YMASSERT below.
*
* @param Name_     -- Name of derived class.
* @param BaseName_ -- Name of base class.
*/
#define YM_HELPER_DECL_YMASSERT2(Name_, BaseName_) \
   class Name_ : public BaseName_          \
   {                                       \
   public:                                 \
      template <typename... Args_T>        \
      explicit inline Name_(               \
            rawstr const Format,           \
            rawstr const File,             \
            uint32 const Line,             \
            Args_T &&... args_uref)        \
         : ymassert_Base(                  \
            Format,                        \
            File,                          \
            Line,                          \
            std::forward<Args_T>(args)...) \
      { }                                  \
   };

#define YM_HELPER_DECL_YMASSERT1(Name_) YM_HELPER_DECL_YMASSERT2(Name_, ymassert_Base)
#define YM_DECL_YMASSERT(...) YM_MACRO_OVERLOAD(YM_DECL_YMASSERT, __VA_ARGS__)

namespace ym
{

/** ymassert_Base
 *
 * @brief Base assert class. Will either set an error flag or throw an exception.
 */
class ymassert_Base
   #if (YM_YES_EXCEPTIONS)
      : public std::exception
   #endif
{
public:
   template <typename... Args_T>
   explicit inline ymassert_Base(
      rawstr const Format,
      rawstr const File,
      uint32 const Line,
      Args_T &&... args_uref);

#if (YM_YES_EXCEPTIONS)
   virtual rawstr what(void) const noexcept override;
#else
   rawstr what(void) const noexcept;
   static void defaultNoExceptHandler(ymassert_Base const & E);
#endif

   static constexpr auto getMaxMsgSize_bytes(void) { return uint64(128u); }

private:
   static std::string format(
      rawstr const     Format,
      fmt::format_args args);

   // TODO use custom allocator
   std::string const _Msg{};
};

/** ymassert_Base
 *
 * @brief Constructor.
 *
 * @note Assert has failed. Print diagnostic information and throw/flag error.
 *
 * @param Format -- Format string.
 * @param File   -- File name of function call.
 * @param Line   -- Line # of function call.
 * @param ...    -- Arguments.
 */
template <typename... Args_T>
inline ymassert_Base::ymassert_Base(
   rawstr const Format,
   rawstr const File,
   uint32 const Line,
   Args_T &&... args_uref) :
      _Msg {format(Format, fmt::make_format_args(File, Line, args_uref...))}
{ }

} // ym
