/**
 * @file    ymassert.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "fmt/base.h"

#include <array>
#include <exception>
#include <type_traits>

/**
 * @brief Convenience macro. Default Assert Handler.
 *    - @ref ym::ymassert_Base::defaultYesExceptHandler()
 *    - @ref ym::ymassert_Base::defaultNoExceptHandler()
 */
#if (YM_EXCEPTIONS_ENABLED)
   #define YM_DAH ymassert_Base::defaultYesExceptHandler(e__)
#else
   #define YM_DAH ymassert_Base::defaultNoExceptHandler(e__)
#endif

/**
 * @brief Convenience macro. Default Assert Handler - Return Error Value.
 *    - @ref ym::ymassert_Base::throwAndReturn()
 *    - @ref ym::ymassert_Base::logAndReturn()
 *
 * @param ReturnVal_ -- The return value.
 */
#if (YM_EXCEPTIONS_ENABLED)
   #define YM_DAH_RV(ReturnVal_) return ymassert_Base::throwAndReturn(e__, ReturnVal_)
#else
   #define YM_DAH_RV(ReturnVal_) return ymassert_Base::logAndReturn(e__, ReturnVal_)
#endif

/**
 * @brief Macro to assert on a condition.
 *
 * - Handlers that return non-void will return it's value from the calling
 *   function. Handlers that return void will call the handler then simply
 *   return from the calling function.
 *
 * - ", ## __VA_ARGS__" eats the comma when `__VA_ARGS__` is empty.
 *
 * - A static assert with @code{cpp} std::is_invocable_v<decltype(Handler_), Derived_> @endcode
 *   doesn't work when Handler_ is a template function because unless the compiler knows it's
 *   specialized type decltype will fail, and providing the machinery to test the proper
 *   invocableness is too messy. If the user provides an invalid Handler function then
 *   the compile error will lead them to this note ... hopefully.
 *
 * - Handler_ will typically be @ref YM_DAH or @ref YM_DAH_RV (defined above). To install your
 *   own handler:
 *
 *   @code{cpp}
 *      [](ymassert_Base const & E) {
 *         ...
 *      }(e__)
 *
 *      // or (if handler returns a value)
 *
 *      [](ymassert_Base const & E, auto && v) -> auto {
 *         ...
 *         return v;
 *      }(e__, <value-to-return>)
 *
 *      // Note here the lambdas are called, not just defined. e__ is the name of the instantiated error.
 *   @endcode
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

/**
 * @brief Optionally enable assert functionality. See @ref YMASSERT.
 */
#if (YM_DEBUG)
   #define YMASSERTDBG(Cond_, Derived_, Handler_, Format_, ...) YMASSERT(Cond_, Derived_, Handler_, Format_, __VA_ARGS__)
#else
   #define YMASSERTDBG(Cond_, Derived_, Handler_, Format_, ...) (void)0;
#endif

/**
 * @brief Declares a custom error class.
 *
 * @param ... -- Args to pass to macro.
 */
#define YM_DECL_YMASSERT(...) YM_MACRO_OVERLOAD(YM_HELPER_DECL_YMASSERT, __VA_ARGS__)

/// @cond INTERNAL
#define YM_HELPER_DECL_YMASSERT1(DerivedName_) YM_HELPER_DECL_YMASSERT2(ymassert_Base, DerivedName_)
#define YM_HELPER_DECL_YMASSERT2(BaseName_, DerivedName_) class DerivedName_ : public BaseName_ { };
/// @endcond

namespace ym
{

/**
 * @brief Base assert class. Will either set an error flag or throw an exception.
 *
 * __Unit Test__
 * - @ref ym::unit::ymassert::TestSuite::Class_ymassert_Base.
 *
 * @test Shall ?
 */
class ymassert_Base : public std::exception
{
public:

#if (YM_EXCEPTIONS_ENABLED)
   virtual rawstr what(void) const noexcept override;
   static inline void defaultYesExceptHandler(auto const & E) { throw E; }
#else // exceptions disabled
   rawstr what(void) const noexcept;
   static void defaultNoExceptHandler(ymassert_Base const & E) noexcept;
#endif

   static void logAssert(ymassert_Base const & E) noexcept;

   /**
    * @brief Logs the error and returns the desired value.
    *
    * @param E -- Thrown exception.
    * @param r -- Return value.
    *
    * @returns auto -- Supplied return value.
    */
   static inline auto logAndReturn(ymassert_Base const & E, auto && r) noexcept {
      logAssert(E); return r;
   }

   /**
    * @brief Throws the error and returns the desired value.
    *
    * - The return is to satisfy lack-of-return-value warnings.
    *
    * @param E -- Thrown exception.
    * @param r -- Return value.
    *
    * @returns auto -- Supplied return value.
    */
   static inline auto throwAndReturn(ymassert_Base const & E, auto && r) noexcept {
      throw E; return r;
   }

   /**
    * @brief Writes message to internal buffer.
    *
    * @param Format -- Format string.
    * @param args   -- Arguments.
    */
   template <typename... Args_T>
   inline void write(
      rawstr const Format,
      Args_T &&... args) noexcept {
         write_Helper(Format, fmt::make_format_args(args...));
   }

   /**
    * @brief Writes message to internal buffer.
    *
    * - Delay calling format to avoid including fmt/format.h in the header file
    *
    * @param Format -- Format string.
    * @param args   -- Arguments.
    */
   void write_Helper(
      rawstr const     Format,
      fmt::format_args args) noexcept;

private:
      std::array<char,
         #if (YM_DEBUG)
            4096uz
         #else
            256uz
         #endif
         > _msg{};
};

} // ym
