/**
 * @file    main.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

// defs come from internal files
#define YM_YES_EXCEPTIONS true
#define YM_NO_EXCEPTIONS  !YM_YES_EXCEPTIONS
#define YM_CPP_STANDARD  20
// #define YM_USE_FMT false
using rawstr = char const *;

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

#if (false)
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
   template <typename... Args_T>
   explicit inline ymassert_Base(
      rawstr const          Format,
      Args_T &&...          args_uref)
      : _Msg {handler(Format, fmt::make_format_args(args_uref...))}
   { }

#if (YM_YES_EXCEPTIONS)
   virtual rawstr what(void) const noexcept override;

   //typedef void (*ActionHandlerFunc)(void);
   // template <typename F>
   // static inline void raise(F f) { f(); }
#endif

   static constexpr auto getMaxMsgSize_bytes(void) { return (unsigned long)(128); }

private:
   static std::string handler(
      rawstr const     Format,
      fmt::format_args args);

   // TODO use custom allocator
   std::string const _Msg{};
};

#if (YM_YES_EXCEPTIONS)
auto ymassert_Base::what(void) const noexcept -> rawstr
{
   return _Msg.c_str();
}
#endif

std::string ymassert_Base::handler(
   rawstr const     Format,
   fmt::format_args args)
{
   std::string msg(getMaxMsgSize_bytes(), '\0');

   auto const Result = fmt::vformat_to_n(
      msg.data(),
      msg.size() - std::size_t(1),
      Format,
      args);
      
   *Result.out = '\0';

   return msg;
}

// template<typename Derived_T, typename... Args_T>
// inline bool ymassert(bool c, rawstr f, Args_T &&... args)
// {
//    if (!c)
//    {
//    #if (YM_YES_EXCEPTIONS)
//       throw Derived_T(f, args...);
//    #else
//       // action_handler(Derived_T(f, args...));
//    #endif
//    }

//    return false;
// }

// if (!Cond_) { Derived_(Format_, __VA_ARGS__).raise(); }
#define YMASSERT(    \
   Action_,          \
   Cond_,            \
   Derived_,         \
   Format_,          \
   ...)              \
   if (!Cond_) { Action_(Derived_(Format_, __VA_ARGS__)); }

class OutOfRangeError : public ymassert_Base
{
public:
   template <typename... Args_T>
   explicit inline OutOfRangeError(
         rawstr const          Format,
         Args_T &&...          args_uref)
      : ymassert_Base(
         Format,
         std::forward<Args_T>(args_uref)...)
   { }

   // virtual void raise(void) const override { /*throw *this;*/ }
};

template <typename T>
inline void defaultAssertHandler(T const & E)
{
   throw E;
}

void test(int i)
{
   YMASSERT(defaultAssertHandler, i >= 0, OutOfRangeError, "Uh oh! i is {}", i);
}

/**
 *  fmt.a       145136 bytes
 *  fmt.so       16080 bytes
 * !fmt          15968 bytes
 *  fmt.so cmake 16248 bytes
 * 
 * 18056
 * 18032
 * 18400
 * 18528
 */
int main(void)
{
   fmt::println("sizeof Derived is {}", sizeof(OutOfRangeError));
   test(-9);
   return 0;
}
