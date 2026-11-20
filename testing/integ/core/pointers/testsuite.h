/**
 * @file    testsuite.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "testsuitebase.h"

namespace ym::integ::core::pointers
{

/**
 * @brief Test suite for the ym::bound / ym::loose pointer wrappers.
 *
 * - Focus: soundness of the wrapper invariants themselves (non-null guarantee,
 *   pointer-based comparisons, checked access, casting rules) - not of any
 *   structure that consumes them.
 */
class TestSuite : public TestSuiteBase
{
public:
   explicit TestSuite(void);
   virtual ~TestSuite(void) = default;

   YM_INTEG_TESTCASE(InteractiveInspection)
   YM_INTEG_TESTCASE(SmokeTest)
   YM_INTEG_TESTCASE(Soundness_BoundConstruction)
   YM_INTEG_TESTCASE(Soundness_BoundComparisons)
   YM_INTEG_TESTCASE(Soundness_LooseNullness)
   YM_INTEG_TESTCASE(Soundness_LooseCheckedAccess)
   YM_INTEG_TESTCASE(Soundness_ComparisonIsPointerBased)
   YM_INTEG_TESTCASE(Soundness_Casting)
};

} // ym::integ::core::pointers
