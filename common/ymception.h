/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <cstdio>
#include <exception>
#include <type_traits>

namespace ym
{

/*
 * Convenience functions.
 * -------------------------------------------------------------------------- */

template <typename    Ymception_T = class Ymception,
          typename... Args_T>
void ymAssert(   bool   const    Condition,
                 str    const    Format,
                 Args_T const... Args);

template <typename    Ymception_T = class Ymception,
          typename... Args_T>
inline
void ymAssertDbg(bool   const    Condition,
                 str    const    Format,
                 Args_T const... Args);

/* -------------------------------------------------------------------------- */

/**
 * @name Ymception
 * 
 * Base class for all custom exceptions in the ym namespace.
 */
class Ymception : public std::exception
{
public:
   template <typename... Args_T>
   explicit Ymception(str    const    Format,
                      Args_T const... Args);

   virtual ~Ymception(void) = default;

   virtual str what(void) const noexcept override;

   template <typename    Ymception_T,
             typename... Args_T>
   friend void ymAssert(bool   const    Condition,
                        str    const    Format,
                        Args_T const... Args); // calls assertHandler()

private:
   void assertHandler(void) const;

   static constexpr auto s_MaxMsgSize_bytes = 256u;
   char _msg[s_MaxMsgSize_bytes];
};

/**
 * @name Ymception
 * 
 * Constructor.
 *
 * @param Format -- Format string
 * @param Args   -- Arguments
 */
template <typename... Args_T>
Ymception::Ymception(str    const    Format,
                     Args_T const... Args)
   : _msg{'\0'}
{
   std::snprintf(_msg, sizeof(_msg), Format, Args...);
}

/**
 * @name what
 * 
 * Returns the saved off message describing the exception.
 *
 * @return str -- The saved off message
 */
auto Ymception::what(void) const -> str
{
   return _msg;
}

/**
 * @name ymAssert
 * 
 * Asserts on the given condition and throws an instance of the desired
 *  exception if the assert fails.
 *
 * @param Condition -- Condition to evaluate (failure if false)
 * @param Format    -- Format string
 * @param Args      -- Arguments
 */
template <typename    Ymception_T = class Ymception,
          typename... Args_T>
void ymAssert(bool   const    Condition,
              str    const    Format,
              Args_T const... Args)
{
   static_assert(std::is_base_of_v<Ymception, Ymception_T>,
                 "Ymception_T must be a Ymception based type");

   if (!Condition)
   { // assert failed
      Ymception_T e(Format, Args...);
      e.assertHandler(); // throws
   }
}

/**
 * @name ymAssertDbg
 * 
 * Asserts on the given condition and throws an instance of the desired
 *  exception if the assert fails. Assert is only enabled if the
 *  debug flag is set.
 *
 * @param Condition -- Condition to evaluate (failure if false)
 * @param Format    -- Format string
 * @param Args      -- Arguments
 */
template <typename    Ymception_T = class Ymception,
          typename... Args_T>
inline void ymAssertDbg(bool   const    Condition,
                        str    const    Format,
                        Args_T const... Args)
{
#if defined(YM_DBG)
   ymAssert<Ymception_T>(Condition, Format, Args...);
#endif // YM_DBG
}

} // ym
