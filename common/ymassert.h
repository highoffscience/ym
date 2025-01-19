/**
 * @file    ymassert.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "loggable.h"
#include "ymdefs.h"

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
   #define YM_TMP_SRC_LOC_CRNT  std::source_location::current(),
   #define YM_TMP_SRC_LOC_PARAM std::source_location const SrcLoc,
   #define YM_TMP_SRC_LOC       SrcLoc,
#else
   #define YM_TMP_SRC_LOC_CRNT
   #define YM_TMP_SRC_LOC_PARAM
   #define YM_TMP_SRC_LOC
#endif

namespace ym
{

/** ymassert_Base
 *
 * @brief Base assert class. Will either set an error flag or throw
 *        an exception.
 */
class ymassert_Base
   #if (YM_YES_EXCEPTIONS)
      : public std::exception
   #endif
{
public:
   explicit ymassert_Base(
      YM_TMP_SRC_LOC_PARAM
      rawstr const Format,
      /*variadic*/ ...);

#if (YM_YES_EXCEPTIONS)
   virtual ~ymassert_Base(void) = default;
   virtual rawstr what(void) const noexcept override;
#endif
};

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
   static_assert(std::is_convertible_v<Condition_, bool>, "Condition must be convertible to bool");                 \
   static_assert(std::is_base_of_v<ymassert_Base, DerivedYmassert_>, "Assert class must be of type ymassert_Base"); \
   static_assert(std::is_same_v<Format_, rawstr>, "Format must be a string literal");                               \
   if (!Condition)                 \
   {                               \
      YM_THROW DerivedYmassert_(   \
         YM_TMP_SRC_LOC_CRNT       \
         Format,                   \
         __VA_ARGS__);             \
   }

/** ymassert_Base
 *
 * @brief Base assert class. Will either set an error flag or throw
 *        an exception.
 */
#define YM_DECL_YMASSERT(Name_)            \
   class Name_                             \
   {                                       \
   public:                                 \
      template <Loggable... Args_T>        \
      explicit inline Name_(               \
            YM_TMP_SRC_LOC_PARAM           \
            rawstr const Format,           \
            Args_T &&... args_uref)        \
         : ymassert_Base(                  \
            YM_TMP_SRC_LOC                 \
            Format,                        \
            std::forward<Args_T>(args)...) \
      {}                                   \
   };

} // ym
