/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"
#include "ymglobals.h"

#include "memio.h" // Structures under test

#include "fmt/format.h"

/**
 * @brief Constructor.
 */
ym::unit::memio::TestSuite::TestSuite(void) :
   TestSuiteBase("MemIO")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
   addTestCase<StackAlloc>();
   addTestCase<StackString>();
}

/**
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::memio::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}

/**
 * @brief Basic integrity test
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::memio::TestSuite::SmokeTest::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}

/**
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::memio::TestSuite::StackAlloc::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
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

/**
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::memio::TestSuite::StackString::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto f = []([[maybe_unused]] bound<StackBuffer_Base> buf) {

   };
   auto sss = "hello"_ssl;
   f(&sss);

   struct StackString : public StackBufferUser
   {
      constexpr StackString(BoundPtr<StackBuffer_Base> const buffer_Ptr) :
         StackBufferUser(buffer_Ptr),
         _internal {buffer_Ptr.get()}
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
   StackString form(&form_buffer);

   // without "My hero!" small strong optimization means no allocation.
   *form = "Go!";
   form->append(" Torchic! My hero!");
   fmt::println("{}", *form);
   *form = "Go! Pumpkaboo! My hero!";
   fmt::println("{}", *form);

   [[maybe_unused]]
   auto const Name = StackStringLiteral("Torchic");

   // [[maybe_unused]]
   // auto buffer = ss.createStackBuffer<100>();

   // TODO also provide a macro for alloca()
   //      YM_STACKBUFFER_DYN_ALLOC(ss, 100);

   return {};
}
