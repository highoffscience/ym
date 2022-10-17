/**
 * @file    ymception.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <array>
#include <cstdio>
#include <exception>
#include <type_traits>

/** YM_DECL_YMEXC
 *
 * @brief Convenience macro to declare custom Ymception classes.
 *
 * @param DerivedYmception_ -- Name of custom Ymception class
 */
#define YM_DECL_YMEXC(DerivedYmception_)                          \
   class DerivedYmception_ : public Ymception                   \
   {                                                            \
   public:                                                      \
      template <typename... Args_T>                             \
      explicit inline DerivedYmception_(str    const    Format, \
                                        Args_T const... Args)   \
         : Ymception(Format, Args...)                           \
      { }                                                       \
                                                                \
      virtual ~DerivedYmception_(void) = default;               \
   };

namespace ym
{

/*
 * Convenience functions.
 * -------------------------------------------------------------------------- */

template <typename    Ymception_T,
          typename... Args_T>
requires(std::is_base_of_v<class Ymception, Ymception_T>)
void ymAssert(bool   const    Condition,
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
   requires(std::is_base_of_v<Ymception, Ymception_T>)
   friend void ymAssert(bool   const    Condition,
                        str    const    Format,
                        Args_T const... Args); // calls assertHandler()

private:
   void assertHandler(void) const;

   std::array<char, 1024u /* max msg size (bytes) */> _msg;
};

/** Ymception
 *
 * @brief Constructor.
 *
 * @tparam Args_T -- Argument types
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
 * @tparam Args_T      -- Argument types
 *
 * @param Condition -- Condition to evaluate (failure if false)
 * @param Format    -- Format string
 * @param Args      -- Arguments
 */
template <typename    Ymception_T,
          typename... Args_T>
requires(std::is_base_of_v<class Ymception, Ymception_T>)
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

} // ym
