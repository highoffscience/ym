/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "lightlogger.h"

#include <exception>

// TODO is this class necessary?
//      need to draw out links with memory pool, textlogger, and ymception

namespace ym
{

/*
 * Convenience functions.
 * -------------------------------------------------------------------------- */

template <typename... Args_T>
inline void ymLightAssert(   bool   const    Condition,
                             str    const    Format,
                             Args_T const... Args);

template <typename... Args_T>
inline void ymLightAssertDbg(bool   const    Condition,
                             str    const    Format,
                             Args_T const... Args);

template <typename... Args_T>
inline void ymLightThrow(    bool   const    Condition,
                             str    const    Format,
                             Args_T const... Args);

template <typename... Args_T>
inline void ymLightThrowDbg( bool   const    Condition,
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
