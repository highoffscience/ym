/**
 * @file    nameable.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "nameable.h"

#include <utility>

/** Nameable
 * 
 * @brief Constructor.
 */
ym::ut::Nameable::Nameable(std::string name)
  : _name {std::move(name)}
{
}
