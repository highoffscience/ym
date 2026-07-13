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
ym::unit::TestSuite::TestSuite(void) :
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
auto ym::unit::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest_Logger);
   return {};
}

/** run
 *
 * @brief Basic integrity test.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::SmokeTest::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);

   struct StackString : public StackBufferUser
   {
      constexpr StackString(BoundPtr<StackBuffer_Base> const buffer_BPtr) :
         StackBufferUser(buffer_BPtr),
         _internal {buffer_BPtr.get()}
      { }

      constexpr auto operator -> (void) noexcept {
         return BoundPtr<std::pmr::string>(&_internal, ym_AssumePtrNotNull{});
      }

      constexpr std::pmr::string & operator * (void) noexcept {
         return _internal;
      }

      std::pmr::string _internal;
   };

   auto form_buffer = StackBuffer<35>();
   StackString form(&form_buffer); // TODO _internal's constructor needs stack buffer, so stack buffer needs
   // to be created before this class. Which means stack buffer's pointer to
   // it's user is free, and the user's pointer to the stack buffer is bound.

   // without "My hero!" small strong optimization means no allocation.
   *form = "Go!";
   form->append(" Torchic! My hero!");
   ymLog(VF::UnitTest, "{}", *form);
   *form = "Go! Pumpkaboo! My hero!";
   ymLog(VF::UnitTest, "{}", *form);

   // [[maybe_unused]]
   // auto buffer = ss.createStackBuffer<100>();

   // TODO also provide a macro for alloca()
   //      YM_STACKBUFFER_DYN_ALLOC(ss, 100);

   return {};
}

/** run
 *
 * @brief TODO.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::StackAlloc::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest_MemIO);

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
