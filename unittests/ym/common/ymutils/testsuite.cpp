/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"
#include "testsuite.h"

#include "ymutils.h"

#include "textlogger.h"
#include "ymassert.h"

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void)
   : TestSuiteBase("YmUtils")
{
   addTestCase<InteractiveInspection>();
   addTestCase<PtrToIntConversion   >();
   addTestCase<BoundedPtrClass      >();
   addTestCase<BinarySearch         >();
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
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::PtrToIntConversion::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmDefs);

   int32 vals[] {9, 7};

   PtrToInt_T p2i_a{vals};
   p2i_a.uint_val += 4u;

   return {
      {"Val", *p2i_a.ptr_val}
   };
}

/** run
 *
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::BoundedPtrClass::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmDefs);

   int a = 9;
   int * a_ptr = &a;

   bptr a_bptr(a_ptr);

   int * b_ptr = a_bptr;

   auto name = "Torchic"_str;

   bptr name_b = name;

   ymLog(VG::UnitTest_YmDefs, "sizeof(TBP) %lu; sizeof(BP) %lu", sizeof(name), sizeof(a_bptr));

   return {
      {"Ptr_1", *b_ptr},
      {"Name", std::string(name_b.get())}
   };
}

/** run
 *
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::BinarySearch::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_YmUtils);

   int values[] = {1, 3, 4, 8, 9, 14, 15, 16, 20};
   auto it = ymBinarySearch(values, values + std::size(values), 4,
      [](auto const Key, auto const * const It_Ptr) -> auto {
         return (Key < *It_Ptr) ? -1 :
                (Key > *It_Ptr) ? +1 : 0;
      }
   );

   return {
      {"ElementFound", it == (values + 2u)}
   };
}
