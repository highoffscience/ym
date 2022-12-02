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
ym::ut::UnitTestBase::UnitTestBase(std::string   && _name_uref,
                                   TCContainer_T && _tcs_uref)
   : _Name      {std::move(_name_uref)},
     _testCases {std::move(_tcs_uref )}
{
}

/** runTestCase
 *
 * @brief TODO.
 */
auto ym::ut::UnitTestBase::runTestCase(std::string const & Name) -> TCDataShuttle_T
{
   for (auto tc : _testCases)
   {
      
   }
}
