/**
 * @file    utception.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "utdefs.h"

#include <exception>
#include <string>
#include <utility>

namespace ym::ut
{

/** YM_UT_DEFN_UTCEPTION
 * 
 * @brief Defines a custom Utception class.
 * 
 * @param Name_ -- Name of class.
 */
#define YM_UT_DEFN_UTCEPTION(Name_)   \
   class Name_ : public Utception     \
   {                                  \
   public:                            \
      explicit Name_(std::string msg) \
         : Utception(std::move(msg))  \
      { }                             \
                                      \
      virtual ~Name_(void) = default; \
   };

/** Utception
 * 
 * @brief Base class for unittest exceptions.
 * 
 * @note cppyy knows how to parse Utception classes but not YmError classes. As such
 *       any potential escapes of YmError needs to be caught and re-thrown as a
 *       Utception.
 */
class Utception : public std::exception
{
public:
   explicit Utception(std::string msg);
   virtual ~Utception(void) = default;

   virtual char const * what(void) const noexcept override;

private:
   std::string const _Msg;
};

} // ym::ut
