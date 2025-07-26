/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"

#include "textlogger.h"
#include "ymglobals.h"

#include "memio.h" // Structures under test

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void) :
   TestSuiteBase("MemIO")
{
   addTestCase<InteractiveInspection>();
   addTestCase<StackAlloc           >();
}

/** run
 *
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_Logger);
   return {};
}

/** run
 *
 * @brief TODO.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::StackAlloc::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_MemIO);

   auto const Block0 = 0xCAFEBABE_u32;

   constexpr auto BuffSize = 10_u32;
   auto * const buffer_Ptr = YM_STACK_ALLOC(uint32, BuffSize);

   for (auto i = 0_u32; i < BuffSize; ++i)
   { // set allocated stack memory
      buffer_Ptr[i] = (i + 1_u32) * 3_u32;
   }

   { // open scoped block

      constexpr auto BuffSize = 10_u32;
      auto * const buffer_Ptr = YM_STACK_ALLOC(uint32, BuffSize);

      for (auto i = 0_u32; i < BuffSize; ++i)
      { // set allocated stack memory
         buffer_Ptr[i] = (i + 1_u32) * 103_u32;
      }
   }

   auto const FirstElementOK = buffer_Ptr[0] == 3_u32;
   auto const BlockByteOK    = Block0 == 0xCAFEBABE_u32;

   return {
      {"FirstElementOK", FirstElementOK},
      {"BlockByteOK",    BlockByteOK   }
   };
}
