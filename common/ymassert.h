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
   #define YM_THROW throw
#else
   #define YM_THROW
#endif

#if (YM_CPP_STANDARD >= 20)
   #include <source_location>
   #define YM_HELPER_SRC_LOC_CURRENT    std::source_location::current()
   #define YM_HELPER_SRC_LOC_PRM(Name_) std::source_location const Name_
   #define YM_HELPER_SRC_LOC_VAR(Name_) Name_
   #define YM_HELPER_FMT_PREFIX         "Assert @ \"{}:{}\": "
#else
   #define YM_HELPER_SRC_LOC_CURRENT
   #define YM_HELPER_SRC_LOC_PRM(Name_)
   #define YM_HELPER_SRC_LOC_VAR(Name_)
   #define YM_HELPER_FMT_PREFIX         "Assert: "
#endif

// if (YMASSERT(
//    i < 0,               // condition
//    ym::catch_all_error, // thrown exception or logged error
//    "Value is {}",       // fmt
//    i))                  // args
// {
//    ymLog("{}", 
//    return 0;
// }

// if (i < 0)
// {
// #if EXC
//    throw ym::catch_all_error("Value is {}", i);
// #else
//    fmt::printf("Value is {}", i);
//    return 0;
// #endif
// }

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
   explicit ymassert_Base(
      rawstr const     Format,
      std::format_args args);

#if (YM_YES_EXCEPTIONS)
   virtual rawstr what(void) const noexcept override;
#endif

   static constexpr auto getMaxMsgSize_bytes(void) { return uint64(128); }

private:
   // TODO use custom allocator
   std::string _msg{};
};

/** ymassert_Base
 *
 * @brief Constructor.
 *
 * @note Assert has failed. Print diagnostic information and throw/flag error.
 * 
 * @todo std::stacktrace.
 *
 * @param SrcLoc -- File location information.
 * @param Format -- Format string.
 * @param ...    -- Arguments.
 */
template <typename... Args_T>
inline ymassert_Base::ymassert_Base(
   bool   const          Cond,
   rawstr const          Format,
   YM_HELPER_SRC_LOC_PRM(SrcLoc),
   Args_T &&...          args_uref)
{
   if (!Cond)
   {
      _msg.resize(getMaxMsgSize_bytes());
      auto const Result = fmt::vformat_to_n(
         _msg.data(),
         _msg.size() - std::size_t(1),
         Format,
         fmt::make_format_args(SrcLoc.file_name(), SrcLoc.line(), args_uref...));
      *Result.out = '\0';
   }
}

/** YMASSERT
 *
 * @brief Macro to assert on a condition.
 *
 * @param Condition_       -- True for happy path, false triggers the assert.
 * @param DerivedYmassert_ -- Ymassert class to handle assert.
 * @param Format_          -- Format string.
 * @param ...              -- Arguments.
 */
#define YMASSERT(Condition_,       \
                 DerivedYmassert_, \
                 Format_,          \
                 ...)              \
   static_assert(std::is_convertible_v<Condition_, bool>,            "Condition must be convertible to bool");      \
   static_assert(std::is_base_of_v<ymassert_Base, DerivedYmassert_>, "Assert class must be of type ymassert_Base"); \
   static_assert(std::is_same_v<Format_, rawstr>,                    "Format must be a string literal");            \
   if (!Condition_)                     \
   {                                    \
      YM_THROW DerivedYmassert_(        \
         YM_HELPER_FMT_PREFIX##Format_, \
         YM_HELPER_SRC_LOC_CURRENT,     \
         __VA_ARGS__);                  \
   }

// TODO add overload so we can have hierarchy 

/** YM_DECL_YMASSERT
 *
 * @brief Declares a custom error class.
 */
#define YM_DECL_YMASSERT(Name_)              \
   class Name_ : public ymassert_Base        \
   {                                         \
   public:                                   \
      template <typename... Args_T>          \
      explicit inline Name_(                 \
            rawstr const          Format,    \
            YM_HELPER_SRC_LOC_PRM(SrcLoc)    \
            Args_T &&...          args_uref) \
         : ymassert_Base(                    \
            Format,                          \
            YM_HELPER_SRC_LOC_VAR(SrcLoc),   \
            std::forward<Args_T>(args)...)   \
      { }                                    \
   };

} // ym
