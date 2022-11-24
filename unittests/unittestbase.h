/**
 * @file    unittestbase.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym_ut.h"

#include <string>
#include <vector>

namespace ym::ut
{

/** UnitTestBase
 *
 * @brief Base class for unit test suites.
 */
class UnitTestBase
{
public:
   explicit UnitTestBase(std::string              && _name_uref,
                         std::vector<std::string> && _dependents_uref);
   virtual ~UnitTestBase(void) = default;

   auto const & getName      (void) const { return _Name;       }
   auto const & getDependents(void) const { return _Dependents; }

private:
   std::string              const _Name;
   std::vector<std::string> const _Dependents;
};

} // ym::ut
