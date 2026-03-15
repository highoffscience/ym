/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"

#include "globallogger.h"
#include "ymglobals.h"

#include "ymdefs.h" // Structures under test

/** TestSuite
 *
 * @brief Constructor.
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
   auto const SE = ymLogPushEnable(VF::UnitTest);
   return {{}};
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
   return {{}};
}

/** run
 *
 * @brief Tests the "big five" delete macros.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::BigFiveDeleteMacros::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);

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
   auto const SE = ymLogPushEnable(VF::UnitTest);

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
   auto const SE = ymLogPushEnable(VF::UnitTest);

   rawstr s0 = "Go! Torchic!";
   uchar  s1 = 7;
   schar  s2 = 9;

   int8 i1 = 1_i8;
   int16 i2 = 1_i16;
   int32 i3 = 1_i32;
   int64 i4 = 1_i64;

   uint8 u1 = 1_u8;
   uint16 u2 = 1_u16;
   uint32 u3 = 1_u32;
   uint64 u4 = 1_u64;

   float32 f1 = 1.0_f32;
   float64 f2 = 1.0_f64;

   return {
      {"Defined", true}
   };
}

/** run
 *
 * @brief Tests basic functions defined in ymdefs.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::Funcs::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);

   auto const I = 0_i32;
   auto const NBits = ym_getNBits<decltype(I)>();

   return {
      {"CorrectNBits", NBits == 32u}
   };
}
