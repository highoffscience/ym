/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"
#include "testsuite.h"

#include "memio.h"

#include "textlogger.h"
#include "ymerror.h"

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void)
   : TestSuiteBase("MemIO")
{
   addTestCase<StackAlloc>();
   addTestCase<PoolAlloc >();
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
auto ym::ut::TestSuite::PoolAlloc::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_MemIO);

   struct Uint128_T
   {
      uint64 high;
      uint64 low;
   };

   auto pool = MemIO::getNewPool<Uint128_T>(6_u64);

   for (auto i = 0_u64; i < pool.getNChunksPerBlock() * 2_u64; ++i)
   {
      auto * const e_Ptr = pool.alloc();
      ymLog(VG::UnitTest_MemIO, "-> %p", e_Ptr);
      if (i%2_u64 == 0_u64)
      {
         pool.dealloc(e_Ptr);
      }
   }

   {
      auto p_uptr = MemIO::alloc_smart<int>(1);
      *p_uptr = 9;
      ymLog(VG::UnitTest_MemIO, "smart = %d", *p_uptr);
   }

   for (auto i = 0_u64; i < pool.getNChunksPerBlock(); i += 2_u64)
   {
      auto e_uptr = pool.alloc_smart();
      auto e2_uptr = pool.alloc_smart();
      ymLog(VG::UnitTest_MemIO, "--> %p", e_uptr.get());
      ymLog(VG::UnitTest_MemIO, "--> %p", e2_uptr.get());
   }

   return {
      {"dummy", true}
   };
}
