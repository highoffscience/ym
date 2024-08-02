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
 * @brief Convenience macro to declare empty custom Ymerror classes.
 * 
 * @note check() instead of assert() to avoid name clashes (like boost's #define assert).
 * 
 * @ref <https://en.cppreference.com/w/cpp/language/class_template_argument_deduction>
 *
 * @param BaseYmerror_    -- Name of base class.
 * @param DerivedYmerror_ -- Name of custom Ymerror class.
 */
#define YM_DECL_YMERROR2(BaseYmerror_, DerivedYmerror_)                             \
                                                                                    \
   static_assert(std::is_base_of_v<Ymerror, BaseYmerror_>,                          \
      #BaseYmerror_" must be of Ymerror type");                                     \
                                                                                    \
   class DerivedYmerror_ : public BaseYmerror_                                      \
   {                                                                                \
   public:                                                                          \
      explicit inline DerivedYmerror_(std::string msg)                              \
         : BaseYmerror_(std::move(msg))                                             \
      { }                                                                           \
                                                                                    \
      virtual ~DerivedYmerror_(void) = default;                                     \
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
               throw DerivedYmerror_(                                               \
                  assertHandler(#DerivedYmerror_,                                   \
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

/** YM_DECL_TAGGED_YMERROR
 *
 * @brief Convenience macro to declare empty custom Ymerror classes.
 * 
 * @note check() instead of assert() to avoid name clashes (like boost's #define assert).
 * 
 * @ref <https://en.cppreference.com/w/cpp/language/class_template_argument_deduction>
 *
 * @param DerivedYmerror_ -- Name of custom Ymerror class.
 * @param ...             -- List of enum fields.
 */
#define YM_DECL_TAGGED_YMERROR(DerivedTaggedYmerror_, ...)                          \
   class DerivedTaggedYmerror_ : public Ymerror                                     \
   {                                                                                \
   public:                                                                          \
      enum Tag_T : uint32 { __VA_ARGS__ };                                          \
                                                                                    \
      explicit inline DerivedTaggedYmerror_(std::string msg,                        \
                                            Tag_T const Tag)                        \
         : Ymerror(std::move(msg), std::to_underlying(Tag))                         \
      { }                                                                           \
                                                                                    \
      virtual ~DerivedTaggedYmerror_(void) = default;                               \
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
               throw DerivedTaggedYmerror_(                                         \
                  assertHandler(#DerivedTaggedYmerror_,                             \
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

/** YM_DECL_YMERROR1
 *
 * @brief Convenience macro to declare empty custom Ymerror classes.
 *
 * @param DerivedYmerror_ -- Name of custom Ymerror class.
 */
#define YM_DECL_YMERROR1(DerivedYmerror_) YM_DECL_YMERROR2(Ymerror, DerivedYmerror_)

namespace ym
{

/** Ymerrorable
 *
 * @brief Represents a Ymerror class.
 *
 * @tparam T -- Type that is or is derived from Ymerror.
 */
template <typename T>
concept Ymerrorable = std::is_base_of_v<class Ymerror, T>;

/** Ymerror
 *
 * @brief Base class for all custom exceptions in the ym namespace.
 * 
 * @ref <https://en.cppreference.com/w/cpp/language/eval_order>. See point 20.
 */
class Ymerror : public std::exception
{
public:
   explicit Ymerror(std::string  msg,
                    uint32 const Tag = 0_u32);
   virtual ~Ymerror(void) = default;

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
