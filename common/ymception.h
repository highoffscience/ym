/**
 * @file    ymception.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "loggable.h"

#include <cstdio>
#include <exception>
#include <source_location>
#include <string>
#include <type_traits>
#include <utility>

/** YM_DECL_YMCEPT
 * 
 * @brief Macro to facilitate macro overloading.
 * 
 * @param ... -- Args to pass to macro.
 */
#define YM_DECL_YMCEPT(...) YM_MACRO_OVERLOAD(YM_DECL_YMCEPT, __VA_ARGS__)

/** YM_DECL_YMCEPT2
 *
 * @brief Convenience macro to declare empty custom Ymception classes.
 * 
 * @note ::check instead of ::assert to avoid name clashes (like boost's #define assert).
 *
 * @param BaseYmception_    -- Name of base class.
 * @param DerivedYmception_ -- Name of custom Ymception class.
 */
#define YM_DECL_YMCEPT2(BaseYmception_, DerivedYmception_)                          \
                                                                                    \
   static_assert(std::is_base_of_v<Ymception, BaseYmception_>,                      \
      #BaseYmception_" must be of Ymception type");                                 \
                                                                                    \
   class DerivedYmception_ : public BaseYmception_                                  \
   {                                                                                \
   public:                                                                          \
      explicit inline DerivedYmception_(std::string && msg_uref)                    \
         : BaseYmception_(std::move(msg_uref))                                      \
      { }                                                                           \
                                                                                    \
      virtual ~DerivedYmception_(void) = default;                                   \
                                                                                    \
      template <Loggable... Args_T>                                                 \
      struct check                                                                  \
      {                                                                             \
         explicit check(                                                            \
            bool                 const    Condition,                                \
            str                  const    Format,                                   \
            Args_T               const... Args,                                     \
            std::source_location const    SrcLoc = std::source_location::current()) \
         {                                                                          \
            if (!Condition)                                                         \
            {                                                                       \
               throw DerivedYmception_(assertHandler(#DerivedYmception_,            \
                                                     SrcLoc,                        \
                                                     Format,                        \
                                                     Args...));                     \
            }                                                                       \
         }                                                                          \
      };                                                                            \
                                                                                    \
      template <typename... Args_T>                                                 \
      check(bool   const    Condition,                                              \
            str    const    Format,                                                 \
            Args_T const... Args) -> check<Args_T...>;                              \
   };

/** YM_DECL_YMCEPT1
 *
 * @brief Convenience macro to declare empty custom Ymception classes.
 *
 * @param DerivedYmception_ -- Name of custom Ymception class.
 */
#define YM_DECL_YMCEPT1(DerivedYmception_) YM_DECL_YMCEPT2(Ymception, DerivedYmception_)

namespace ym
{

/** Ymceptable
 *
 * @brief Represents a Ymception class.
 *
 * @tparam T -- Type that is or is derived from Ymception.
 */
template <typename T>
concept Ymceptable = std::is_base_of_v<class Ymception, T>;

/** Ymception
 *
 * @brief Base class for all custom exceptions in the ym namespace.
 */
class Ymception : public std::exception
{
public:
   explicit inline Ymception(std::string && msg_uref);

   virtual ~Ymception(void) = default;

   virtual str what(void) const noexcept override;

protected:
   static std::string assertHandler(str                  const Name,
                                    std::source_location const SrcLoc,
                                    str                  const Format,
                                    /*variadic*/               ...);

private:
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
inline Ymception::Ymception(std::string && msg_uref)
   : _Msg {std::move(msg_uref)}
{
}

} // ym
