/**
 * @file    utception.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ut.h"

#include <exception>
#include <string>
#include <utility>

namespace ut
{

/** UT_DEFN_UTCEPTION
 * 
 * @brief Defines a custom Utception class.
 * 
 * @param Name_ -- Name of class.
 */
#define UT_DEFN_UTCEPTION(Name_)      \
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
 */
class Utception : public std::exception
{
public:
   explicit Utception(std::string msg);
   virtual ~Utception(void) = default;

   virtual utstr what(void) const noexcept override;

private:
   std::string const _Msg;
};

} // ut
