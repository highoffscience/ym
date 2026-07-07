/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"

#include "globallogger.h"
#include "ymglobals.h"

#include "stackbuffer.hpp" // Structures under test

#include <array>
#include <string>

/** TestSuite
 *
 * @brief Constructor.
 */
ym::unit::TestSuite::TestSuite(void) :
   TestSuiteBase("StackBuffer")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
}

/** run
 *
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);
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

      constexpr BoundPtr<std::pmr::string> operator -> (void) noexcept {
         return &_internal;
      }

      std::pmr::string _internal;
   };

   auto form_buffer = StackBuffer<100>();
   StackString form(&form_buffer); // TODO _internal's constructor needs stack buffer, so stack buffer needs
   // to be created before this class. Which means stack buffer's pointer to
   // it's user is free, and the user's pointer to the stack buffer is bound.

   // [[maybe_unused]]
   // auto buffer = ss.createStackBuffer<100>();

   // TODO also provide a macro for alloca()
   //      YM_STACKBUFFER_DYN_ALLOC(ss, 100);

   return {};
}
