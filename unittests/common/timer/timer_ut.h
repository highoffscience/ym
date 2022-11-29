/**
 * @file    timer_ut.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym_ut.h"

#include "unittestbase.h"

namespace ym::ut
{

/** Timer_UT
*
* @brief Test suite for structure Timer.
*/
class Timer_UT : public UnitTestBase
{
public:
   explicit Timer_UT(void);
   virtual ~Timer_UT(void) = default;

   // YM_UT_TESTCASE(name_of_test_case_here);
};

} // ym::ut
