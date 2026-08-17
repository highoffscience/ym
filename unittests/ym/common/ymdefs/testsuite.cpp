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
#include <type_traits>

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
   addTestCase<PrimitiveDefSuffixes>();
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

/**
 * - @ref YM_USER_LITERAL_DECL() Shall define int8 user defined literal.
 * - @ref YM_USER_LITERAL_DECL() Shall define int16 user defined literal.
 * - @ref YM_USER_LITERAL_DECL() Shall define int32 user defined literal.
 * - @ref YM_USER_LITERAL_DECL() Shall define int64 user defined literal.
 *
 * - @ref YM_USER_LITERAL_DECL() Shall define uint8 user defined literal.
 * - @ref YM_USER_LITERAL_DECL() Shall define uint16 user defined literal.
 * - @ref YM_USER_LITERAL_DECL() Shall define uint32 user defined literal.
 * - @ref YM_USER_LITERAL_DECL() Shall define uint64 user defined literal.
 *
 * - @ref YM_USER_LITERAL_DECL() Shall define float32 user defined literal.
 * - @ref YM_USER_LITERAL_DECL() Shall define float64 user defined literal.
 * - @ref YM_USER_LITERAL_DECL() Shall define floatext user defined literal.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::PrimitiveDefSuffixes::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   rawstr s  = "Go! Torchic!";
   uchar  uc = 7;
   schar  sc = 9;

   auto i8  = 1_i8;
   auto i16 = 1_i16;
   auto i32 = 1_i32;
   auto i64 = 1_i64;

   auto u8  = 1_u8;
   auto u16 = 1_u16;
   auto u32 = 1_u32;
   auto u64 = 1_u64;

   auto f32  = 1.0_f32;
   auto f64  = 1.0_f64;
   auto fext = 1.0_fext;

   constexpr auto TypesAreOfExpectedTypes =
      std::conjunction_v<
         std::is_same<decltype(s),  char const * >,
         std::is_same<decltype(uc), unsigned char>,
         std::is_same<decltype(sc), signed char  >,

         std::is_same<decltype(i8 ), std::int8_t >,
         std::is_same<decltype(i16), std::int16_t>,
         std::is_same<decltype(i32), std::int32_t>,
         std::is_same<decltype(i64), std::int64_t>,

         std::is_same<decltype(u8 ), std::uint8_t >,
         std::is_same<decltype(u16), std::uint16_t>,
         std::is_same<decltype(u32), std::uint32_t>,
         std::is_same<decltype(u64), std::uint64_t>,

         // precision of floats checked at definition site
         std::is_same<decltype(f32 ), float32 >,
         std::is_same<decltype(f64 ), float64 >,
         std::is_same<decltype(fext), floatext>
      >;

   return {
      {"TypesAreOfExpectedTypes", TypesAreOfExpectedTypes}
   };
}
