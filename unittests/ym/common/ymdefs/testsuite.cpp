/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"
#include "ymglobals.h"

#include "ymdefs.h" // Structures under test

#include "fmt/format.h"

#include <cstdint>

/**
 * @brief Constructor.
 */
ym::unit::TestSuite::TestSuite(void) :
   TestSuiteBase("YmDefs")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
   addTestCase<BigFiveDeleteMacros>();
   addTestCase<OverloadMacros>();
   addTestCase<Func_getNBits>();
   addTestCase<Func_empty>();
   addTestCase<PrimitiveDefs>();
}

/**
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}

/**
 * @brief Basic integrity test.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::SmokeTest::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}

/**
 * - @ref YM_NO_DEFAULT() Shall delete the default constructor.
 * - @ref YM_NO_COPY() Shall delete the copy constructor.
 * - @ref YM_NO_ASSIGN() Shall delete the assignment operator.
 * - @ref YM_NO_MOVE_COPY() Shall delete the move constructor.
 * - @ref YM_NO_MOVE_ASSIGN() Shall delete the move operator.
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

/**
 * - @ref YM_MACRO_OVERLOAD() Shall allow custom macros to have overloads.
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

/**
 * - @ref ym::ym_getNBits() Shall return 16 when called with an i16 (2-byte) type.
 * - @ref ym::ym_getNBits() Shall return 32 when called with an i32 (4-byte) type.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::Func_getNBits::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const I16 = std::int16_t{};
   auto const Ni16Bits = ym_getNBits<decltype(I16)>();

   auto const I32 = std::int32_t{};
   auto const Ni32Bits = ym_getNBits<decltype(I32)>();

   return {
      {"CorrectNBits", (Ni16Bits == 2uz) && (Ni32Bits == 4uz)}
   };
}

/**
 * - @ref ym::ym_empty() Shall return true when passed a nullptr.
 * - @ref ym::ym_empty() Shall return true when passed an empty string.
 * - @ref ym::ym_empty() Shall return false when passed the string "Torchic".
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::Func_empty::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {
      {"CorrectResults",
         (ym_empty(nullptr)   == true) &&
         (ym_empty("")        == true) &&
         (ym_empty("Torchic") == false)
      }
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
