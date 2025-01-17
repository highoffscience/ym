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
#endif

#if (YM_CPP_STANDARD >= 20)
   #include <source_location>
#endif

namespace ym
{

class ymassert_Base
   #if (YM_YES_EXCEPTIONS)
      : public std::exception
   #endif
{
public:
   explicit ymassert_Base(
      #if (YM_CPP_STANDARD >= 20)
         std::source_location const SrcLoc,
      #endif
         rawstr               const Format,
         /*variadic*/               ...);

#if (YM_YES_EXCEPTIONS)
   virtual ~ymassert_Base(void) = default;

   virtual rawstr what(void) const noexcept override;
#endif
};

#if (YM_YES_EXCEPTIONS)
   #define YM_THROW throw
#else // YM_NO_EXCEPTIONS
   #define YM_THROW
#endif

#define YMASSERT(Condition_,              \
                 DerivedYmassert_,        \
                 Format_,                 \
                 ...)                     \
   if (!Condition)                        \
   {                                      \
      YM_THROW DerivedYmassert_(          \
         std::source_location::current(), \
         Format,                          \
         __VA_ARGS__);                    \
   }

#define YM_DECL_YMASSERT(Name_) \
   class Name_
   {
   public:
      template <Loggable...
   }

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
