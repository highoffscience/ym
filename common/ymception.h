/**
 * @file    ymception.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "loggable.h"

#include <array>
#include <cstdio>
#include <exception>
#include <type_traits>

/**
 * TODO
 */
#define YM_DECL_YMCEPT(BaseYmception_, DerivedYmception_)       \
   static_assert(std::is_base_of_v<Ymception, BaseYmception_>,  \
      #BaseYmception_" must be of Ymception type");             \
   class DerivedYmception_ : public BaseYmception_              \
   {                                                            \
   public:                                                      \
      template <Loggable... Args_T>                             \
      explicit inline DerivedYmception_(str    const    Format, \
                                        Args_T const... Args)   \
         : BaseYmception_(Format, Args...)                      \
      { }                                                       \
                                                                \
      virtual ~DerivedYmception_(void) = default;               \
                                                                \
      virtual str getName(void) const override {                \
         return #DerivedYmception_;                             \
      }                                                         \
   };

/** YM_DECL_YMCEPT
 *
 * @brief Convenience macro to declare empty custom Ymception classes.
 *
 * @param DerivedYmception_ -- Name of custom Ymception class.
 */
#define YM_DECL_YMCEPT_BASE(DerivedYmception_) \
   YM_DECL_YMCEPT(Ymception, DerivedYmception_)

namespace ym
{

/** Ymceptable
 *
 * @brief Represents a Ymception class.
 *
 * @tparam T -- Type that is derived from Ymception.
 */
template <typename T>
concept Ymceptable =  std::is_base_of_v<class Ymception, T> &&
                     !std::is_same_v<   class Ymception, T>;

/*
 * Convenience functions.
 * -------------------------------------------------------------------------- */

template <Ymceptable  DerivedYmception_T,
          Loggable... Args_T>
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
   template <Loggable... Args_T>
   explicit inline Ymception(str    const    Format,
                             Args_T const... Args);

   virtual ~Ymception(void) = default;

   virtual str getName(void) const = 0;

   virtual str what(void) const noexcept override;

   template <Ymceptable  Ymception_T,
             Loggable... Args_T>
   friend void ymAssert(bool   const    Condition,
                        str    const    Format,
                        Args_T const... Args);

private:
   void assertHandler(void) const;

   std::array<char, 1024_u64 /* max msg size (bytes) */> _msg;
};

/** Ymception
 *
 * @brief Constructor.
 *
 * @tparam Args_T -- Argument types.
 *
 * @param Format -- Format string.
 * @param Args   -- Arguments.
 */
template <Loggable... Args_T>
inline Ymception::Ymception([[maybe_unused]] str    const    Format,
                            [[maybe_unused]] Args_T const... Args)
   : _msg {'\0'}
{
   // TODO I get a format security warning here - Format isn't a string literal.
   //      Investigate when converting to the fmt library
   std::snprintf(_msg.data(), _msg.size(), getName());
   //std::snprintf(_msg.data(), _msg.size(), Format, Args...);
   // std::snprintf(_msg.data(), _msg.size(), "Why, hello there!");
}

/** ymAssert
 *
 * @brief Asserts on the given condition and throws an instance of the desired
 *        exception if the assert fails.
 *
 * @tparam Ymception_T -- Ymception based type.
 * @tparam Args_T      -- Argument types.
 *
 * @param Condition -- Condition to evaluate (failure if false).
 * @param Format    -- Format string.
 * @param Args      -- Arguments.
 */
template <Ymceptable  DerivedYmception_T,
          Loggable... Args_T>
void ymAssert(bool   const    Condition,
              str    const    Format,
              Args_T const... Args)
{
   if (!Condition)
   { // assert failed
      DerivedYmception_T e(Format, Args...);
      e.assertHandler(); // throws
   }
}

} // ym
