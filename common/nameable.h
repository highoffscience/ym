/**
 * @file    nameable.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 * 
 * @note File used in unittests - maximum standard C++20.
 */

#pragma once

#include "ymdefs.h"

#include <string>
#include <utility>

namespace ym
{

/*
 * Available variants:
 *
 * class Nameable_NV
 * class PermaNameable_NV
 */

/** Nameable_NV
 * 
 * @brief Provides a mutably named object.
 * 
 * @note NV = Non-Virtual
 * 
 * @tparam String_T -- String-like class.
 */
template <typename String_T = std::string>
class Nameable_NV
{
public:
   /** Nameable_NV
    * 
    * @brief Constructor.
    * 
    * @param name -- Initial name.
    */
   explicit inline Nameable_NV(String_T name) :
      _name {std::move(name)}
   { }

   /** getName
    *
    * @brief Returns the current name.
    * 
    * @returns auto const & -- Current name.
    */
   inline auto const & getName(void) const { return _name; }

   /** setName
    * 
    * @brief Sets the current name.
    * 
    * @param name -- Desired name.
    */
   inline void setName(String_T name) { _name = std::move(name); }

private:
   String_T _name{};
};

/** PermaNameable_NV
 * 
 * @brief Provides a permanently named object.
 * 
 * @note NV = Non-Virtual
 * 
 * @tparam String_T -- String-like class.
 */
template <typename String_T = std::string>
class PermaNameable_NV
{
public:
   /** PermaNameable_NV
    * 
    * @brief Constructor.
    * 
    * @param name -- Initial name.
    */
   explicit inline PermaNameable_NV(String_T name) :
      _Name {std::move(name)}
   { }

   /** getName
    *
    *  @brief Returns the current name.
    * 
    * @returns auto const & -- Current name.
    */
   inline auto const & getName(void) const { return _Name; }

private:
   String_T const _Name{};
};

} // ym
