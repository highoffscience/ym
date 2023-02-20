/**
 * @file    ymception.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "loggable.h"

#include <cstdio>
#include <exception>
#include <source_location>
#include <string>
#include <type_traits>

/**
 * TODO
 */
#define YM_DECL_YMCEPT(...) YM_MACRO_OVERLOAD(__VA_ARGS__, YM_DECL_YMCEPT2, \
                                                           YM_DECL_YMCEPT1  )(__VA_ARGS__)

/**
 * TODO
 */
#define YM_DECL_YMCEPT2(BaseYmception_, DerivedYmception_)      \
   static_assert(std::is_base_of_v<Ymception, BaseYmception_>,  \
      #BaseYmception_" must be of Ymception type");             \
   class DerivedYmception_ : public BaseYmception_              \
   {                                                            \
   public:                                                      \
      explicit inline DerivedYmception_(std::string const Name, \
                                        std::string const Msg)  \
         : BaseYmception_(Name, Msg)                            \
      { }                                                       \
                                                                \
      virtual ~DerivedYmception_(void) = default;               \
                                                                \
      static constexpr auto getClassName(void) {                \
         return #DerivedYmception_;                             \
      }                                                         \
   };

/** YM_DECL_YMCEPT1
 *
 * @brief Convenience macro to declare empty custom Ymception classes.
 *
 * @param DerivedYmception_ -- Name of custom Ymception class.
 */
#define YM_DECL_YMCEPT1(DerivedYmception_) \
   YM_DECL_YMCEPT2(Ymception, DerivedYmception_)

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

// TODO <https://stackoverflow.com/questions/14805192/c-variadic-template-function-parameter-with-default-value>
template <Loggable... Args_T>
struct YmceptionAssertHelper
{

};

template <Ymceptable DerivedYmception_T>
void ymAssert(bool                 const Condition,
              std::string          const Msg,
              std::source_location const SrcLoc = std::source_location::current());

/* -------------------------------------------------------------------------- */

/** Ymception
 *
 * @brief Base class for all custom exceptions in the ym namespace.
 */
class Ymception : public std::exception
{
public:
   explicit inline Ymception(std::string const Name,
                             std::string const Msg);

   virtual ~Ymception(void) = default;

   virtual str what(void) const noexcept override;

   template <Ymceptable DerivedYmception_T>
   friend void ymAssert(bool                 const Condition,
                        std::string          const Msg,
                        std::source_location const SrcLoc);

private:
   void assertHandler(void) const;

   std::string const _Msg;
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
inline Ymception::Ymception(std::string const Name,
                            std::string const Msg)
   : _Msg {Name + ": " + Msg + "!"}
{
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
template <Ymceptable DerivedYmception_T>
void ymAssert(bool                 const Condition,
              std::string          const Msg,
              std::source_location const SrcLoc)
{
   if (!Condition)
   { // assert failed
      DerivedYmception_T e(DerivedYmception_T::getClassName(), Msg);
      e.assertHandler(); // throws
   }
}

} // ym
