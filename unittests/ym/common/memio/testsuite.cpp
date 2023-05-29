/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"
#include "testsuite.h"

#include "memio.h"

#include "textlogger.h"
#include "ymception.h"

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void)
   : TestSuiteBase("MemIO")
{
   addTestCase<StackAlloc>();
   addTestCase<HeapAlloc >();
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

/** run
 *
 * @brief TODO.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::HeapAlloc::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_MemIO);

   auto data_uptr = MemIO::alloc_safe<uint32>(10_u64);
   ymLog(VG::UnitTest_MemIO, "data_uptr %p", data_uptr.get());

   return {
      {"dummy", true}
   };
}
