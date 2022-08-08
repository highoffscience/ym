/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "textlogger.h"

#include <atomic>
#include <exception>
#include <string>
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

template <typename... Args_T>
inline void ymAssert(   bool   const    Condition,
                        str    const    Format,
                        Args_T const... Args);

template <typename... Args_T>
inline void ymAssertDbg(bool   const    Condition,
                        str    const    Format,
                        Args_T const... Args);

template <typename... Args_T>
inline void ymThrow(    bool   const    Condition,
                        str    const    Format,
                        Args_T const... Args);

template <typename... Args_T>
inline void ymThrowDbg( bool   const    Condition,
                        str    const    Format,
                        Args_T const... Args);

/* -------------------------------------------------------------------------- */

/**
 * TODO should we have a global logger? Or register the global logger?
 */
class Ymception : public std::exception
{
public:
   explicit Ymception(str const Msg);

   virtual ~Ymception(void) = default;

   template <typename... Args_T>
   friend void ymAssert(bool   const    Condition,
                        TextLogger &    txtlog_ref,
                        str    const    Format,
                        Args_T const... Args);

   template <typename... Args_T>
   friend void ymThrow(bool   const    Condition,
                       TextLogger &    txtlog_ref,
                       str    const    Format,
                       Args_T const... Args);

   inline auto getTag(void) const { return _Tag; }

private:
   template <typename... Args_T>
   static void assertPrintToLogAndConditionallyThrow(bool   const    Condition,
                                                     bool   const    TerminateOnFailure,
                                                     TextLogger &    txtlog_ref,
                                                     str    const    Format,
                                                     Args_T const... Args);

   static std::atomic<uint32> _s_tagCount;

   uint32 const _Tag;
};

/**
 *
 */
template <typename... Args_T>
inline void ymAssert(bool   const    Condition,
                     TextLogger &    txtlog_ref,
                     str    const    Format,
                     Args_T const... Args)
{
   Ymception::assertPrintToLogAndConditionallyThrow(Condition, true, txtlog_ref, Format, Args...);
}

/**
 *
 */
template <typename... Args_T>
inline void ymAssertDbg(bool   const    Condition,
                        TextLogger &    txtlog_ref,
                        str    const    Format,
                        Args_T const... Args)
{
#if defined(YM_DBG)
   ymAssert(Condition, txtlog_ref, Format, Args...);
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
 *
 */
template <typename... Args_T>
void Ymception::assertPrintToLogAndConditionallyThrow(bool   const    Condition,
                                                      bool   const    TerminateOnFailure,
                                                      TextLogger &    txtlog_ref,
                                                      str    const    Format,
                                                      Args_T const... Args)
{
   if (!Condition)
   { // assert failed
      txtlog_ref.printf(0, "Assert failed!");

      Ymception e("Assertion failure!");
      if (TerminateOnFailure)
      { // let user know we are about to terminate
         ymLog(0, "About to terminate!");
      }
      else
      { // let user know we are about to throw
         ymLog(0, "Throwing Ymception with tag %lu.", e.getTag());
      }

      ymLog(0, Format, Args...);

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

      if (TerminateOnFailure)
      { // terminate
         std::terminate();
      }
      else
      { // throw
         throw e;
      }
   }
}

} // ym
