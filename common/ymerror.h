/**
 * @file    ymerror.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "loggable.h"
#include "ymdefs.h"

#include <cstdio>
#include <string>
#include <type_traits>
#include <utility>

#if defined(YM_USE_EXCEPTIONS)
   #include <exception>
#else
   #include <array>
#endif

#if (YM_CPP_STANDARD >= 20)
   #include <source_location>
#endif

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
#if (YM_CPP_STANDARD >= 20)
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
#else // YM_CPP_STANDARD < 20
   #define YM_DECL_YMERROR2(BaseYmError_, DerivedYmError_)    \
                                                              \
      static_assert(std::is_base_of_v<YmError, BaseYmError_>, \
         #BaseYmError_" must be of YmError type");            \
                                                              \
      class DerivedYmError_ : public BaseYmError_             \
      {                                                       \
      public:                                                 \
         explicit inline DerivedYmError_(std::string msg)     \
            : BaseYmError_(std::move(msg))                    \
         { }                                                  \
                                                              \
         virtual ~DerivedYmError_(void) = default;            \
                                                              \
         template <typename... Args_T>                        \
         struct check                                         \
         {                                                    \
            explicit check(                                   \
               bool   const    Condition,                     \
               rawstr const    Format,                        \
               Args_T const... Args)                          \
            {                                                 \
               if (!Condition)                                \
               {                                              \
                  throw DerivedYmError_(                      \
                     assertHandler(#DerivedYmError_,          \
                                   Format,                    \
                                   Args...));                 \
               }                                              \
            }                                                 \
         };                                                   \
      };
#endif

/** YM_DECL_YMERROR1
 *
 * @brief Convenience macro to declare empty custom YmError classes.
 *
 * @param DerivedYmError_ -- Name of custom YmError class.
 */
#define YM_DECL_YMERROR1(DerivedYmError_) YM_DECL_YMERROR2(YmError, DerivedYmError_)

namespace ym
{

/** YmError
 *
 * @brief Base class for all custom exceptions in the ym namespace.
 * 
 * @ref <https://en.cppreference.com/w/cpp/language/eval_order>. See point 20.
 */
class YmError : public std::exception
{
public:
   explicit YmError(std::string msg);
   virtual ~YmError(void) = default;

   virtual rawstr what(void) const noexcept override;

   static bool isErrorRaised(void);
   static std::string getRaisedError(void);

protected:
   static std::string assertHandler(
      rawstr               const Name,
   #if (YM_CPP_STANDARD >= 20)
      std::source_location const SrcLoc,
   #endif
      rawstr               const Format,
      /*variadic*/               ...);

private:
   void consume(void); // TODO we need a static array to hold all raised errors

   std::string const _Msg;

#if !defined(YM_USE_EXCEPTIONS) // no exceptions
   using RaisedErrorArray_T = std::array<std::string, 10_u32>;
   static inline    RaisedErrorArray_T _s_raisedErrors {     };
   static inline    bool               _s_raisedError  {false};
#endif
};

} // ym
