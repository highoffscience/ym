/**
 * @file    unittestbase.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "unittestbase.h"

#include <utility>

/** UnitTestBase
 *
 * @brief Constructor.
 */
ym::ut::UnitTestBase::UnitTestBase(std::string && _name_uref)
   : _Name {std::move(_name_uref)}
{
}

/** TestCase
 * 
 * @brief Constructor.
 */
ym::ut::UnitTestBase::TestCase::TestCase(std::string && _name_uref)
  : _Name {std::move(_name_uref)}
{
}
