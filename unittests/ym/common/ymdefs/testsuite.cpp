/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"
#include "ymglobals.h"

#include "ymdefs.h" // Structures under test

#include "fmt/format.h"

/** TestSuite
 *
 * @brief Constructor.
 *
 * @throws Whatever addTestCase() throws.
 */
ym::unit::TestSuite::TestSuite(void) :
   TestSuiteBase("YmDefs")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
   addTestCase<BigFiveDeleteMacros>();
   addTestCase<OverloadMacros>();
   addTestCase<PrimitiveDefs>();
   addTestCase<Funcs>();
}

/** run
 *
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
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
   return {};
}

/** run
 *
 * @brief Tests the "big five" delete macros.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::BigFiveDeleteMacros::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   struct Test
   {
      YM_NO_DEFAULT    (Test)
      YM_NO_COPY       (Test)
      YM_NO_ASSIGN     (Test)
      YM_NO_MOVE_COPY  (Test)
      YM_NO_MOVE_ASSIGN(Test)
   };

   return {
      {"Defined", true}
   };
}

/** run
 *
 * @brief Tests overload macro hack.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::OverloadMacros::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   #define YM_SUM(...) YM_MACRO_OVERLOAD(YM_SUM, __VA_ARGS__)
   #define YM_SUM1(First) (First)
   #define YM_SUM2(First, Second) (YM_SUM1(First) + YM_SUM1(Second))
   #define YM_SUM3(First, Second, Third) (YM_SUM2(First, Second) + YM_SUM1(Third))
   #define YM_SUM4(First, Second, Third, Fourth) (YM_SUM3(First, Second, Third) + YM_SUM1(Fourth))

   auto const Sum = YM_SUM(1, 2, 3, 4);

   return {
      {"DefinedAndWorks", Sum == (1+2+3+4)}
   };
}

/** run
 *
 * @brief Tests that primitive sized defs are defined.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::PrimitiveDefs::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   [[maybe_unused]] volatile rawstr s0 = "Go! Torchic!";
   [[maybe_unused]] volatile uchar  s1 = 7;
   [[maybe_unused]] volatile schar  s2 = 9;

   [[maybe_unused]] volatile int8 i1 = 1_i8;
   [[maybe_unused]] volatile int16 i2 = 1_i16;
   [[maybe_unused]] volatile int32 i3 = 1_i32;
   [[maybe_unused]] volatile int64 i4 = 1_i64;

   [[maybe_unused]] volatile uint8 u1 = 1_u8;
   [[maybe_unused]] volatile uint16 u2 = 1_u16;
   [[maybe_unused]] volatile uint32 u3 = 1_u32;
   [[maybe_unused]] volatile uint64 u4 = 1_u64;

   [[maybe_unused]] volatile float32 f1 = 1.0_f32;
   [[maybe_unused]] volatile float64 f2 = 1.0_f64;

   return {
      {"Defined", true}
   };
}

/**
 * - @ref ym::ym_getNBits() Shall return 16 when called with an i16 (2-byte) type.
 * - @ref ym::ym_getNBits() Shall return 32 when called with an i32 (4-byte) type.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::Test_getNBits::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const I = 0_i32;
   auto const NBits = ym_getNBits<decltype(I)>();

   return {
      {"CorrectNBits", NBits == 32uz}
   };
}
