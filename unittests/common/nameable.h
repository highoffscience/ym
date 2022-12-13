/**
 * @file    nameable.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ut.h"

#include <string>
#include <utility>

namespace ym::ut
{

/** Nameable
 * 
 * @brief Represents a nameable object.
 */
class Nameable
{
public:
   explicit Nameable(std::string name);
   virtual ~Nameable(void) = default;

   inline auto const & getName(void) const { return _name; }

   inline void setName(std::string name) { _name = std::move(name); }

private:
   std::string _name;
};

} // ym::ut
