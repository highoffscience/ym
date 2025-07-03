/**
 * @file    nameable.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "nameable.h"

/** Nameable
 * 
 * @brief Constructor.
 */
ym::ut::Nameable::Nameable(std::string name)
  : _name {std::move(name)}
{
}
