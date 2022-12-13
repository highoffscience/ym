/**
 * @file    testcase.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testcase.h"

#include <utility>

/** TestCase
 * 
 * @brief Constructor.
 */
ym::ut::TestCase::TestCase(std::string name)
  : _Name {std::move(name)}
{
}
