/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <array>
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
//requires(std::is_base_of_v<Ymception, Ymception_T>)
void ymAssert(   bool   const    Condition,
                 str    const    Format,
                 Args_T const... Args);

template <typename    Ymception_T = class Ymception,
          typename... Args_T>
//requires(std::is_base_of_v<Ymception, Ymception_T>)
inline
void ymAssertDbg(bool   const    Condition,
                 str    const    Format,
                 Args_T const... Args);

/* -------------------------------------------------------------------------- */

/** Ymception
 * 
 * @brief Base class for all custom exceptions in the ym namespace.
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

   std::array<char, 256u /* max msg size (bytes) */> _msg;
};

/** Ymception
 * 
 * @brief Constructor.
 *
 * @tparam Args_T -- List of argument types
 * 
 * @param Format -- Format string
 * @param Args   -- Arguments
 */
template <typename... Args_T>
Ymception::Ymception(str    const    Format,
                     Args_T const... Args)
   : _msg{'\0'}
{
   std::snprintf(_msg.data(), _msg.size(), Format, Args...);
}

/** ymAssert
 * 
 * @brief Asserts on the given condition and throws an instance of the desired
 *        exception if the assert fails.
 *
 * @tparam Ymception_T -- Ymception based type
 * @tparam Args_T      -- List of argument types
 * 
 * @param Condition -- Condition to evaluate (failure if false)
 * @param Format    -- Format string
 * @param Args      -- Arguments
 */
template <typename    Ymception_T,
          typename... Args_T>
void ymAssert(bool   const    Condition,
              str    const    Format,
              Args_T const... Args)
{
   if (!Condition)
   { // assert failed
      Ymception_T e(Format, Args...);
      e.assertHandler(); // throws
   }
}

/** ymAssertDbg
 * 
 * @brief Asserts on the given condition and throws an instance of the desired
 *        exception if the assert fails. Assert is only enabled if the
 *        debug flag is set.
 *
 * @tparam Ymception_T -- Ymception based type
 * @tparam Args_T      -- List of argument types
 * 
 * @param Condition -- Condition to evaluate (failure if false)
 * @param Format    -- Format string
 * @param Args      -- Arguments
 */
template <typename    Ymception_T = Ymception,
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
