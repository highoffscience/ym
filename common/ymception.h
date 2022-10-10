/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "textlogger.h"

#include <cstdio>
#include <exception>
#include <type_traits>
#include <utility>

#if defined(YM_DBG)
#include <boost/stacktrace.hpp>
#endif // YM_DBG

namespace ym
{

/*
 * Convenience functions.
 * -------------------------------------------------------------------------- */

template <typename    DerivedYmception_T,
          typename... Args_T>
void ymAssert(          bool   const    Condition,
                        str    const    Format,
                        Args_T const... Args);

template <typename... Args_T>
inline void ymAssertDbg(bool   const    Condition,
                        str    const    Format,
                        Args_T const... Args);

/* -------------------------------------------------------------------------- */

/**
 * TODO should we have a global logger? Or register the global logger?
 */
class Ymception : public std::exception
{
public:
   template <typename... Args_T>
   explicit Ymception(str    const    Format,
                      Args_T const... Args);

   virtual ~Ymception(void) = default;

   virtual str what(void) const noexcept override;

   void assertHandler(void) const;

private:
   static constexpr auto s_MaxMsgSize_bytes = 256u;
   char _msg[s_MaxMsgSize_bytes];
};

/**
 *
 */
template <typename... Args_T>
Ymception::Ymception(str    const    Format,
                     Args_T const... Args)
   : _msg{'\0'}
{
   std::snprintf(_msg, sizeof(_msg), Format, Args...);
}

/**
 * 
 */
auto Ymception::what(void) const -> str
{
   return _msg;
}

/**
 *
 */
template <typename    DerivedYmception_T,
          typename... Args_T>
void ymAssert(bool   const    Condition,
              str    const    Format,
              Args_T const... Args)
{
   static_assert(std::is_base_of_v<Ymception, DerivedYmception_T>, "Derived must be a Ymception based type");

   if (!Condition)
   { // assert failed
      DerivedYmception_T e(Format, Args...);
      e.assertHandler();
   }
}

/**
 *
 */
template <typename... Args_T>
inline void ymAssertDbg(bool   const    Condition,
                        str    const    Format,
                        Args_T const... Args)
{
#if defined(YM_DBG)
   ymAssert(Condition, Format, Args...);
#endif // YM_DBG
}

/**
 *
 */
template <typename... Args_T>
inline void ymThrow(bool   const    Condition,
                    TextLogger &    txtlog_ref,
                    str    const    Format,
                    Args_T const... Args)
{
   Ymception::assertPrintToLogAndConditionallyThrow(Condition, false, txtlog_ref, Format, Args...);
}

/**
 *
 */
template <typename... Args_T>
inline void ymThrowDbg(bool   const    Condition,
                       TextLogger &    txtlog_ref,
                       str    const    Format,
                       Args_T const... Args)
{
#if defined(YM_DBG)
   ymThrow(Condition, txtlog_ref, Format, Args...);
#endif // YM_DBG
}

/**
 * TODO replace verbose level from 0 with correct mask
 * TODO moce to cpp
 */
void Ymception::assertHandler(void) const
{
   ymLog(0, "Assert failed!");
   ymLog(0, what());

#if defined(YM_DBG)
   ymLog(0, "Stack dump follows...");

   { // split and print stack dump
      std::string const StackDumpStr = boost::stacktrace::to_string(boost::stacktrace::stacktrace());

      for (auto startPos = StackDumpStr.find_first_not_of('\n', 0);
            startPos != std::string::npos;
            /*empty*/)
      { // print each line of the stack dump separately
         auto const EndPos = StackDumpStr.find_first_of('\n', startPos);
         ymLog(0, StackDumpStr.substr(startPos, EndPos - startPos).c_str());
         startPos = StackDumpStr.find_first_not_of('\n', EndPos);
      }
   }
#endif // YM_DBG

   throw *this;
}

} // ym
