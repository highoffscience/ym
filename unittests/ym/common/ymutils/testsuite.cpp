/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"

#include "textlogger.h"
#include "ymglobals.h"

#include "ymutils.h" // Structures under test

#include <string>

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void) :
   TestSuiteBase("YmUtils")
{
   addTestCase<InteractiveInspection>();
   addTestCase<PtrToIntConversion   >();
   addTestCase<BoundedPtrClass      >();
   addTestCase<BinarySearch         >();
   addTestCase<BoundedStr           >();
   addTestCase<PtrCast              >();
   addTestCase<BitSet               >();
}

/** run
 *
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmUtils);

   return {};
}

/** run
 *
 * @brief TODO
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::PtrToIntConversion::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmUtils);

   int32 vals[] {9, 7};

   PtrToInt_T p2i_a{vals};
   p2i_a.uint_val += 4u;

   return {
      {"Val", *p2i_a.ptr_val}
   };
}

/** run
 *
 * @brief TODO
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::BoundedPtrClass::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmUtils);

   int a = 9;
   int * a_ptr = &a;

   bptr a_bptr(a_ptr);

   int * b_ptr = a_bptr;

   auto name = "Torchic"_str;

   bptr name_b = name;

   ymLog(VG::UnitTest_YmUtils, "sizeof(TBP) {}; sizeof(BP) {}", sizeof(name), sizeof(a_bptr));

   return {
      {"Ptr_1", *b_ptr},
      {"Name", std::string(name_b.get())}
   };
}

/** run
 *
 * @brief TODO
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::BinarySearch::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmUtils);

   int values[] = {1, 3, 4, 8, 9, 14, 15, 16, 20};
   auto it = ymBinarySearch(values, values + std::size(values), 4,
      [](auto const Key, auto const * const It_Ptr) -> auto {
         return
            (Key < *It_Ptr) ? -1 :
            (Key > *It_Ptr) ? +1 : 0;
      }
   );

   return {
      {"ElementFound", it == (values + 2u)}
   };
}

/** run
 *
 * @brief TODO
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::BoundedStr::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmUtils);

   using namespace std::string_literals;

   return {
   };
}

/** run
 *
 * @brief TODO
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::PtrCast::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmUtils);

   int i = 9;
   int * p1 = &i;
   int const * p2 = &i;

   auto * bytes1 = ymCastPtrTo<uint8>(p1);
   auto * bytes2 = ymCastPtrTo<uint8 const>(p2);

   // compile error (expected)
   // auto * bytes3 = ymCastPtrTo<uint8>(p2);

   static_assert(!std::is_const_v<std::remove_pointer_t<decltype(bytes1)>>, "bytes1 expected to be non-const");
   static_assert( std::is_const_v<std::remove_pointer_t<decltype(bytes2)>>, "bytes2 expected to be const");

   return {
      {"True", true}
   };
}

/** run
 *
 * @brief TODO
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::BitSet::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmUtils);

   Bitset b{};
   ymLog(VG::UnitTest_YmUtils, "1) TODO --> {}", b.getUnderlying());
   b.set(0);
   ymLog(VG::UnitTest_YmUtils, "2) TODO --> {}", b.getUnderlying());
   b.set(1);
   ymLog(VG::UnitTest_YmUtils, "3) TODO --> {}", b.getUnderlying());
   b.clear(1);
   ymLog(VG::UnitTest_YmUtils, "4) TODO --> {}", b.getUnderlying());
   b.set(1, true);
   ymLog(VG::UnitTest_YmUtils, "5) TODO --> {}", b.getUnderlying());
   auto b2 = b;
   ymLog(VG::UnitTest_YmUtils, "6) TODO --> {}", b2.getUnderlying());

   return {
      {"True", true}
   };
}
