/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym_unittest.h"

#include "random.h"
#include "unittestbase.h"

namespace ym::unittest
{

// --- TODO ---

struct UT_Random : public Random
{

};

// ------------

/**
 * <https://www.systutorials.com/docs/linux/man/1-dieharder/>
 */
class Random_UnitTest : public UnitTestBase
{
public:
   explicit Random_UnitTest(void);
   virtual ~Random_UnitTest(void) = default;

   virtual void runTests(void) override;

private:
   Random _rand;
};

} // ym::unittest
