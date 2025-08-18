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
   /// @brief Convenience macro. Default Assert Handler.
   #define YM_DAH ymassert_Base::defaultYesExceptHandler(e__)
#else
   /// @brief Convenience macro. Default Assert Handler.
   #define YM_DAH ymassert_Base::defaultNoExceptHandler(e__)
#endif

/// @brief Convenience macro. Default Assert Handler - Return Error Value.
#define YM_DAH_REV(ReturnVal_) return ymassert_Base::logAndReturn(e__, ReturnVal_)

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
 * @note Handler_ will typically be YM_DAH or YM_DAH_REV (defined above). To install your own handler:
 * 
 *       [](ymassert_Base const & E) {
 *          ...
 *       }(e__)
 * 
 *       or
 * 
 *       [](ymassert_Base const & E, auto && v) -> auto {
 *          ...
 *          return v;
 *       }(e__, <value-to-return>)
 * 
 *       Note here the lambdas are called, not just defined. e__ is the name of the instantiated error.
 * 
 * @param Cond_    -- Condition - true for happy path, false triggers the assert.
 * @param Derived_ -- Ymassert class to handle assert.
 * @param Handler_ -- Evaluated callback function if assert fails. Must be callable with Ymassert_Base.
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
      Handler_;                                                     \
   }

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

   static void logAssert(ymassert_Base const & E);
   static inline auto logAndReturn(ymassert_Base const & E, auto && v_uref) {
      logAssert(E); return v_uref;
   }

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
