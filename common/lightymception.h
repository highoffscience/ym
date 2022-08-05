/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "lightlogger.h"

#include <exception>

namespace ym
{

/*
 * Convenience functions.
 * -------------------------------------------------------------------------- */

template <typename... Args_T>
inline void ymLightAssert(   bool   const    Condition,
                        TextLogger &    txtlog_ref,
                        str    const    Format,
                        Args_T const... Args);

template <typename... Args_T>
inline void ymLightAssertDbg(bool   const    Condition,
                        TextLogger &    txtlog_ref,
                        str    const    Format,
                        Args_T const... Args);

template <typename... Args_T>
inline void ymLightThrow(    bool   const    Condition,
                        TextLogger &    txtlog_ref,
                        str    const    Format,
                        Args_T const... Args);

template <typename... Args_T>
inline void ymLightThrowDbg( bool   const    Condition,
                        TextLogger &    txtlog_ref,
                        str    const    Format,
                        Args_T const... Args);

/* -------------------------------------------------------------------------- */

/**
 *
 */
class LightYmception : public std::exception
{
public:
   explicit LightYmception(str const Msg);

   virtual ~LightYmception(void);
};

} // ym
