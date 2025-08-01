/**
 * @file    ymassert.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "fmt/base.h"

#include <type_traits>

#if (YM_YES_EXCEPTIONS)
   #include <exception>
#endif

#if (YM_YES_EXCEPTIONS)
   #define YM_DEFAULT_ASSERT_HANDLER ymassert_Base::defaultYesExceptHandler
#else
   #define YM_DEFAULT_ASSERT_HANDLER ymassert_Base::defaultNoExceptHandler
#endif

/// @brief Convenience macro.
#define YM_DAH YM_DEFAULT_ASSERT_HANDLER

/** YMASSERT
 *
 * @brief Macro to assert on a condition.
 *
 * @note Handlers that return non-void will return it's value from the calling
 *       function. Handlers that return void will call the handler then simply
 *       return from the calling function.
 *
 * @note ", ## __VA_ARGS__" eats the comma when __VA_ARGS__ is empty.
 * 
 * @note A static assert with std::is_invocable_v<decltype(Handler_), Derived_> doesn't work
 *       when Handler_ is a template function because unless the compiler knows it's
 *       specialized type decltype will fail, and providing the machinery to test the proper
 *       invocableness is too messy. If the user provides an invalid Handler function then
 *       the compile error will lead them to this note...hopefully.
 * 
 * @param Cond_    -- Condition - true for happy path, false triggers the assert.
 * @param Derived_ -- Ymassert class to handle assert.
 * @param Handler_ -- Callback function if assert fails. Must be callable with Ymassert_Base.
 * @param Format_  -- Format string. Must be a string literal.
 * @param ...      -- Arguments.
 */
#define YMASSERT(                                                   \
      Cond_,                                                        \
      Derived_,                                                     \
      Handler_,                                                     \
      Format_,                                                      \
      ...)                                                          \
   static_assert(std::is_convertible_v<decltype(Cond_), bool>,      \
      "Condition must be convertible to bool");                     \
   static_assert(std::is_base_of_v<ymassert_Base, Derived_>,        \
      "Assert class must be of type ymassert_Base");                \
   if (!(Cond_))                                                    \
   {                                                                \
      Derived_ e__;                                                 \
      e__.write("Assert @ \"{}:{}\": " Format_,                     \
         __FILE__, __LINE__, ## __VA_ARGS__);                       \
      Handler_(e__);                                                \
   }

/** YMASSERTDBG
 *
 * @brief Macro to assert on a condition. Enabled only if YM_DEBUG is enabled.
 *        This version should be used for logic errors, as it is meant to be
 *        disabled for production.
 *
 * @param Cond_    -- Condition - true for happy path, false triggers the assert.
 * @param Derived_ -- Ymassert class to handle assert.
 * @param Handler_ -- Callback function if assert fails.
 * @param Format_  -- Format string.
 * @param ...      -- Arguments.
 */
#if (YM_DEBUG)
   #define YMASSERTDBG(Cond_, Derived_, Handler_, Format_, ...) YMASSERT(Cond_, Derived_, Handler_, Format_, __VA_ARGS__)
#else
   #define YMASSERTDBG(Cond_, Derived_, Handler_, Format_, ...) (void)0;
#endif

/** YM_DECL_YMASSERT
*
* @brief Declares a custom error class.
*
* @param Name_     -- Name of derived class.
* @param BaseName_ -- Name of base class.
*/
#define YM_DECL_YMASSERT(...) YM_MACRO_OVERLOAD(YM_HELPER_DECL_YMASSERT, __VA_ARGS__)
#define YM_HELPER_DECL_YMASSERT1(Name_) YM_HELPER_DECL_YMASSERT2(ymassert_Base, Name_)
#define YM_HELPER_DECL_YMASSERT2(BaseName_, Name_) class Name_ : public BaseName_ { };

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

#if (YM_YES_EXCEPTIONS)
   virtual rawstr what(void) const noexcept override;
   static inline void defaultYesExceptHandler(auto const & E) { throw E; }
#else // YM_NO_EXCEPTIONS
   rawstr what(void) const noexcept;
   static void defaultNoExceptHandler(ymassert_Base const & E);
#endif

   static constexpr auto _s_MaxMsgSize_bytes = std::size_t(128u);

   template <typename... Args_T>
   inline void write(
      rawstr const Format,
      Args_T &&... args_uref) {
         write_Helper(Format, fmt::make_format_args(args_uref...));
   }

   // delay calling format to avoid including fmt/format.h in the header file
   void write_Helper(
      rawstr const     Format,
      fmt::format_args args);

private:
   char _msg[_s_MaxMsgSize_bytes]{'\0'};
};

} // ym
