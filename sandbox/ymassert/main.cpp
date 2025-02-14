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
#include "fmt/chrono.h"

#include <chrono>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>

#if (YM_YES_EXCEPTIONS)
   #include <exception>
#else
   #include <csignal>
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
   // virtual void raise(void) const = 0;
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
   if (!(Cond_)) { Action_(Derived_(Format_, __VA_ARGS__)); }

#define YM_DECL_YMASSERT(Name_) \
   class Name_ : public ymassert_Base \
   { \
   public: \
      template <typename... Args_T> \
      explicit inline Name_( \
            rawstr const          Format, \
            Args_T &&...          args_uref) \
         : ymassert_Base( \
            Format, \
            std::forward<Args_T>(args_uref)...) \
      { } \
   };

// 

YM_DECL_YMASSERT(OutOfRangeError)
YM_DECL_YMASSERT(OutOfRangeError0)
YM_DECL_YMASSERT(OutOfRangeError1)
YM_DECL_YMASSERT(OutOfRangeError2)
YM_DECL_YMASSERT(OutOfRangeError3)
YM_DECL_YMASSERT(OutOfRangeError4)
YM_DECL_YMASSERT(OutOfRangeError5)
YM_DECL_YMASSERT(OutOfRangeError6)
YM_DECL_YMASSERT(OutOfRangeError7)
YM_DECL_YMASSERT(OutOfRangeError8)
YM_DECL_YMASSERT(OutOfRangeError9)

#define YM_DEFAULT_ASSERT_HANDLER throw

template <typename T>
void defaultAssertHandler(T const & E)
{
   throw E;
   // std::raise(SIGTERM);
}

void test(int i)
{
   static_assert(std::is_convertible_v<decltype(i != 10), bool>);
   YMASSERT(YM_DEFAULT_ASSERT_HANDLER, i != 10, OutOfRangeError,  "Uh oh!  i is {}", i);
   YMASSERT(YM_DEFAULT_ASSERT_HANDLER, i !=  0, OutOfRangeError0, "Uh oh!0 i is {}", i);
   YMASSERT(YM_DEFAULT_ASSERT_HANDLER, i !=  1, OutOfRangeError1, "Uh oh!1 i is {}", i);
   YMASSERT(YM_DEFAULT_ASSERT_HANDLER, i !=  2, OutOfRangeError2, "Uh oh!2 i is {}", i);
   YMASSERT(YM_DEFAULT_ASSERT_HANDLER, i !=  3, OutOfRangeError3, "Uh oh!3 i is {}", i);
   YMASSERT(YM_DEFAULT_ASSERT_HANDLER, i !=  4, OutOfRangeError4, "Uh oh!4 i is {}", i);
   YMASSERT(YM_DEFAULT_ASSERT_HANDLER, i !=  5, OutOfRangeError5, "Uh oh!5 i is {}", i);
   YMASSERT(YM_DEFAULT_ASSERT_HANDLER, i !=  6, OutOfRangeError6, "Uh oh!6 i is {}", i);
   YMASSERT(YM_DEFAULT_ASSERT_HANDLER, i !=  7, OutOfRangeError7, "Uh oh!7 i is {}", i);
   YMASSERT(YM_DEFAULT_ASSERT_HANDLER, i !=  8, OutOfRangeError8, "Uh oh!8 i is {}", i);
   YMASSERT(YM_DEFAULT_ASSERT_HANDLER, i !=  9, OutOfRangeError9, "Uh oh!9 i is {}", i);
}

/**
 *  fmt.a       145136 bytes
 *  fmt.so       16080 bytes
 * !fmt          15968 bytes
 *  fmt.so cmake 16248 bytes
 * 
 * 23560 with no op
 * 29240 with std::raise action
 * 29488 with direct throw statement
 * 34504 with virtual raise
 * 29752 with defaultHandler
 */
int main(void)
{
   volatile int i = 0;
   fmt::println("sizeof Derived is {} {}", sizeof(OutOfRangeError), __FILE__);
   // test(((unsigned long)(void*)&i) % 11u);
   using namespace std::literals::chrono_literals;
   auto diff = std::chrono::microseconds((3600 * 25 * 1'000'000ll) + (15 * 60 * 1'000'000ll));
   fmt::print("{:%Q}\n", diff);
   return 0;
}
