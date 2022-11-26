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
 * 
 * @note We initialize the dependents per test suite instead of per test case to ensure
 *       that the test suite of all dependents is run. If it were on a per test case basis
 *       then we would have to query every functions dependents list - too error prone!
 */
ym::ut::UnitTestBase::UnitTestBase(std::string              && _name_uref,
                                   std::vector<std::string> && _dependents_uref)
   : _Name       {std::move(_name_uref      )},
     _Dependents {std::move(_dependents_uref)}
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
