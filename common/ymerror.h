/**
 * @file    ymerror.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 * 
 */

#pragma once

#include "ymdefs.h"

#include "loggable.h"

#include <cstdio>
#include <exception>
#include <source_location>
#include <string>
#include <type_traits>

/** YM_DECL_YMERROR
 * 
 * @brief Macro to facilitate macro overloading.
 * 
 * @param ... -- Args to pass to macro.
 */
#define YM_DECL_YMERROR(...) YM_MACRO_OVERLOAD(YM_DECL_YMERROR, __VA_ARGS__)

/** YM_DECL_YMERROR2
 *
 * @brief Convenience macro to declare empty custom YmError classes.
 * 
 * @note check() instead of assert() to avoid name clashes (like boost's #define assert).
 * 
 * @ref <https://en.cppreference.com/w/cpp/language/class_template_argument_deduction>
 *
 * @param BaseYmError_    -- Name of base class.
 * @param DerivedYmError_ -- Name of custom YmError class.
 */
#define YM_DECL_YMERROR2(BaseYmError_, DerivedYmError_)                             \
                                                                                    \
   static_assert(std::is_base_of_v<YmError, BaseYmError_>,                          \
      #BaseYmError_" must be of YmError type");                                     \
                                                                                    \
   class DerivedYmError_ : public BaseYmError_                                      \
   {                                                                                \
   public:                                                                          \
      explicit inline DerivedYmError_(std::string msg)                              \
         : BaseYmError_(std::move(msg))                                             \
      { }                                                                           \
                                                                                    \
      virtual ~DerivedYmError_(void) = default;                                     \
                                                                                    \
      template <Loggable... Args_T>                                                 \
      struct check                                                                  \
      {                                                                             \
         explicit check(                                                            \
            bool                 const    Condition,                                \
            rawstr               const    Format,                                   \
            Args_T               const... Args,                                     \
            std::source_location const    SrcLoc = std::source_location::current()) \
         {                                                                          \
            if (!Condition)                                                         \
            {                                                                       \
               throw DerivedYmError_(                                               \
                  assertHandler(#DerivedYmError_,                                   \
                                SrcLoc,                                             \
                                Format,                                             \
                                Args...));                                          \
            }                                                                       \
         }                                                                          \
      };                                                                            \
                                                                                    \
      template <typename... Args_T>                                                 \
      check(bool   const    Condition,                                              \
            rawstr const    Format,                                                 \
            Args_T const... Args) -> check<Args_T...>;                              \
   };

/** YM_DECL_YMERROR1
 *
 * @brief Convenience macro to declare empty custom YmError classes.
 *
 * @param DerivedYmError_ -- Name of custom YmError class.
 */
#define YM_DECL_YMERROR1(DerivedYmError_) YM_DECL_YMERROR2(YmError, DerivedYmError_)

/** YM_DECL_TAGGED_YMERROR
 *
 * @brief Convenience macro to declare empty custom YmError classes.
 * 
 * @note check() instead of assert() to avoid name clashes (like boost's #define assert).
 * 
 * @ref <https://en.cppreference.com/w/cpp/language/class_template_argument_deduction>
 *
 * @param DerivedYmError_ -- Name of custom YmError class.
 * @param ...             -- List of enum fields.
 */
#define YM_DECL_TAGGED_YMERROR(DerivedTaggedYmError_, ...)                          \
   class DerivedTaggedYmError_ : public YmError                                     \
   {                                                                                \
   public:                                                                          \
      enum Tag_T : uint32 { __VA_ARGS__ };                                          \
                                                                                    \
      explicit inline DerivedTaggedYmError_(std::string msg,                        \
                                            Tag_T const Tag)                        \
         : YmError(std::move(msg), std::to_underlying(Tag))                         \
      { }                                                                           \
                                                                                    \
      virtual ~DerivedTaggedYmError_(void) = default;                               \
                                                                                    \
      template <Loggable... Args_T>                                                 \
      struct check                                                                  \
      {                                                                             \
         explicit check(                                                            \
            Tag_T                const    Tag,                                      \
            bool                 const    Condition,                                \
            rawstr               const    Format,                                   \
            Args_T               const... Args,                                     \
            std::source_location const    SrcLoc = std::source_location::current()) \
         {                                                                          \
            if (!Condition)                                                         \
            {                                                                       \
               throw DerivedTaggedYmError_(                                         \
                  assertHandler(#DerivedTaggedYmError_,                             \
                                SrcLoc,                                             \
                                Format,                                             \
                                Args...), Tag);                                     \
            }                                                                       \
         }                                                                          \
      };                                                                            \
                                                                                    \
      template <Loggable... Args_T>                                                 \
      check(Tag_T  const    Tag,                                                    \
            bool   const    Condition,                                              \
            rawstr const    Format,                                                 \
            Args_T const... Args) -> check<Args_T...>;                              \
   };

namespace ym
{

/** YmErrorable
 *
 * @brief Represents a YmError class.
 *
 * @tparam T -- Type that is or is derived from YmError.
 */
template <typename T>
concept YmErrorable = std::is_base_of_v<class YmError, T>;

/** YmError
 *
 * @brief Base class for all custom exceptions in the ym namespace.
 * 
 * @ref <https://en.cppreference.com/w/cpp/language/eval_order>. See point 20.
 */
class YmError : public std::exception
{
public:
   explicit YmError(std::string  msg,
                    uint32 const Tag = 0_u32);
   virtual ~YmError(void) = default;

   virtual rawstr what(void) const noexcept override;

   inline auto getTag(void) const { return _Tag; }

protected:
   static std::string assertHandler(
      rawstr               const Name,
      std::source_location const SrcLoc,
      rawstr               const Format,
      /*variadic*/               ...);

private:
   std::string const _Msg;
   uint32      const _Tag;
};

} // ym
