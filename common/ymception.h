/**
 * @file    ymception.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "loggable.h"

#include <cstdio>
#include <exception>
#include <source_location>
#include <string>
#include <type_traits>

/** YM_DECL_YMCEPT
 * 
 * @brief Macro to facilitate macro overloading.
 * 
 * @param ... -- Args to pass to macro.
 */
#define YM_DECL_YMCEPT(...) YM_MACRO_OVERLOAD(YM_DECL_YMCEPT, __VA_ARGS__)

/** YM_DECL_YMCEPT2
 *
 * @brief Convenience macro to declare empty custom Ymception classes.
 * 
 * @note check() instead of assert() to avoid name clashes (like boost's #define assert).
 * 
 * @ref <https://en.cppreference.com/w/cpp/language/class_template_argument_deduction>
 *
 * @param BaseYmception_    -- Name of base class.
 * @param DerivedYmception_ -- Name of custom Ymception class.
 */
#define YM_DECL_YMCEPT2(BaseYmception_, DerivedYmception_)                          \
                                                                                    \
   static_assert(std::is_base_of_v<Ymception, BaseYmception_>,                      \
      #BaseYmception_" must be of Ymception type");                                 \
                                                                                    \
   class DerivedYmception_ : public BaseYmception_                                  \
   {                                                                                \
   public:                                                                          \
      explicit inline DerivedYmception_(std::string msg)                            \
         : BaseYmception_(std::move(msg))                                           \
      { }                                                                           \
                                                                                    \
      virtual ~DerivedYmception_(void) = default;                                   \
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
               throw DerivedYmception_(                                             \
                  assertHandler(#DerivedYmception_,                                 \
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

/** YM_DECL_TAGGED_YMCEPT
 *
 * @brief Convenience macro to declare empty custom Ymception classes.
 * 
 * @note check() instead of assert() to avoid name clashes (like boost's #define assert).
 * 
 * @ref <https://en.cppreference.com/w/cpp/language/class_template_argument_deduction>
 *
 * @param DerivedYmception_ -- Name of custom Ymception class.
 * @param ...               -- List of enum fields.
 */
#define YM_DECL_TAGGED_YMCEPT(DerivedTaggedYmception_, ...)                         \
   class DerivedTaggedYmception_ : public Ymception                                 \
   {                                                                                \
   public:                                                                          \
      enum class Tag_T : uint32 { __VA_ARGS__ };                                    \
                                                                                    \
      explicit inline DerivedTaggedYmception_(std::string msg,                      \
                                              Tag_T const Tag)                      \
         : Ymception(std::move(msg), std::to_underlying(Tag))                       \
      { }                                                                           \
                                                                                    \
      virtual ~DerivedTaggedYmception_(void) = default;                             \
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
               throw DerivedTaggedYmception_(                                       \
                  assertHandler(#DerivedTaggedYmception_,                           \
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

/** YM_DECL_YMCEPT1
 *
 * @brief Convenience macro to declare empty custom Ymception classes.
 *
 * @param DerivedYmception_ -- Name of custom Ymception class.
 */
#define YM_DECL_YMCEPT1(DerivedYmception_) YM_DECL_YMCEPT2(Ymception, DerivedYmception_)

namespace ym
{

/** Ymceptable
 *
 * @brief Represents a Ymception class.
 *
 * @tparam T -- Type that is or is derived from Ymception.
 */
template <typename T>
concept Ymceptable = std::is_base_of_v<class Ymception, T>;

/** Ymception
 *
 * @brief Base class for all custom exceptions in the ym namespace.
 * 
 * @ref <https://en.cppreference.com/w/cpp/language/eval_order>. See point 20.
 */
class Ymception : public std::exception
{
public:
   explicit Ymception(std::string  msg,
                      uint32 const Tag = 0_u32);
   virtual ~Ymception(void) = default;

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
