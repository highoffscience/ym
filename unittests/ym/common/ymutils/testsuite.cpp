/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"

#include "globallogger.h"
#include "ymglobals.h"

#include "ymutils.h" // Structures under test

#include <string>
#include <vector>

/** TestSuite
 *
 * @brief Constructor.
 */
ym::unit::TestSuite::TestSuite(void) :
   TestSuiteBase("YmUtils")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
   addTestCase<Funcs>();
   addTestCase<PtrToIntConversion>();
   addTestCase<BoundedPtrClass>();
   addTestCase<BinarySearch>();
   addTestCase<BoundedStr>();
   addTestCase<PtrCast>();
   addTestCase<BitSet>();
   addTestCase<PolyRawTest>();
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
 * @brief Basic integrity test.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::Funcs::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);

   auto castPtrToSuccess = false;
   {
      int a = 9;
      auto * p = &a;
      auto * u = ym_castPtrTo<void>(p);
      auto * v = ym_castPtrTo<int const>(u);
      castPtrToSuccess  = std::is_void_v<std::remove_pointer_t<decltype(u)>>;
      castPtrToSuccess &= std::is_same_v<int const*, decltype(v)>;
   }

   auto emptySuccess = false;
   {
      rawstr s = nullptr;
      emptySuccess = ym_empty(s);

      s = "";
      emptySuccess &= ym_empty(s);

      s = "Go! Torchic!";
      emptySuccess &= !ym_empty(s);
   }

   auto binarySearchSuccess = false;
   {
      std::vector v = {1,2,3,4,5,6,7,8,9};
      auto e = ym_binarySearch(v.cbegin(), v.cend(), 5);
      binarySearchSuccess = (*e == 5);

      e = ym_binarySearch(v.cbegin(), v.cend(), 10);
      binarySearchSuccess &= (e == v.cend());
   }

   return {
      {"castPtrToSuccess", castPtrToSuccess},
      {"emptySuccess", emptySuccess},
      {"binarySearchSuccess", binarySearchSuccess}
   };
}

/** run
 *
 * @brief TODO
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::PtrToIntConversion::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);

   int32 vals[] {9, 7};

   PtrInt_T p2i_a{vals};
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
auto ym::unit::TestSuite::BoundedPtrClass::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);

   // int a = 9;
   // int * a_ptr = &a;

   // bptr a_bptr(a_ptr);

   // int * b_ptr = a_bptr;

   // auto name = "Torchic"_str;

   // bptr name_b = name;

   // ymLog(VF::UnitTest, "sizeof(TBP) {}; sizeof(BP) {}", sizeof(name), sizeof(a_bptr));

   // {
   //    auto a = 9;
   //    auto const ca = 11;
   //    bptr myint(&a);
   //    bptr mycint(&ca);

   //    [[maybe_unused]] bptr<void>       myvoid_1  = myint;
   //    [[maybe_unused]] bptr<void const> mycvoid_1 = mycint;
   //    [[maybe_unused]] bptr<void const> mycvoid_2 = myint;
   //    // [[maybe_unused]] bptr<void>       myvoid_2  = mycint; // compile error (expected)

   //    [[maybe_unused]] auto mybyte_1 = bptr<byte>(myint, BPtrCastingPassKey());
   //    // [[maybe_unused]] auto myvoid_3 = bptr<void>(mycint); // compile error (expected)

   //    // ymLog(VF::UnitTest, "myvoid's value {}", *static_cast<int const*>(myvoid.get()));
   // }

   // return {
   //    {"Ptr_1", *b_ptr},
   //    {"Name", std::string(name_b.get())}
   // };

   return {{}};
}

/** run
 *
 * @brief TODO
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::BinarySearch::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);

   // int values[] = {1, 3, 4, 8, 9, 14, 15, 16, 20};
   // auto it = ym_binarySearch(values, values + std::size(values), 4,
   //    [](auto const Key, auto const * const It_Ptr) -> auto {
   //       return
   //          (Key < *It_Ptr) ? -1 :
   //          (Key > *It_Ptr) ? +1 : 0;
   //    }
   // );

   // return {
   //    {"ElementFound", it == (values + 2u)}
   // };

   return {{}};
}

/** run
 *
 * @brief TODO
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::BoundedStr::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);

   using namespace std::string_literals;

   return {{}};
}

/** run
 *
 * @brief TODO
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::PtrCast::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);

   int i = 9;
   int * p1 = &i;
   int const * p2 = &i;

   auto * bytes1 = ym_castPtrTo<uint8>(p1);
   auto * bytes2 = ym_castPtrTo<uint8 const>(p2);

   // compile error (expected)
   // auto * bytes3 = ym_castPtrTo<uint8>(p2);

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
auto ym::unit::TestSuite::BitSet::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);

   // Bitset b{};
   // ymLog(VF::UnitTest, "1) TODO --> {}", b.getUnderlying());
   // b.set(0);
   // ymLog(VF::UnitTest, "2) TODO --> {}", b.getUnderlying());
   // b.set(1);
   // ymLog(VF::UnitTest, "3) TODO --> {}", b.getUnderlying());
   // b.clear(1);
   // ymLog(VF::UnitTest, "4) TODO --> {}", b.getUnderlying());
   // b.set(1, true);
   // ymLog(VF::UnitTest, "5) TODO --> {}", b.getUnderlying());
   // auto b2 = b;
   // ymLog(VF::UnitTest, "6) TODO --> {}", b2.getUnderlying());

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
auto ym::unit::TestSuite::PolyRawTest::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VF::UnitTest);

   // struct Base
   // {
   //    virtual ~Base(void) = default;
   //    virtual void cloneAt(bptr<void> const val_BPtr, sizet const Size_bytes) const = 0;
   //    int _i{};
   // };

   // struct Derived : public Base
   // {
   //    virtual void cloneAt(bptr<void> const val_BPtr, [[maybe_unused]] sizet const Size_bytes) const override
   //    {
   //       ::new (val_BPtr.get()) Derived();
   //    }
   // };

   // std::vector<PolyRaw<Base, sizeof(Derived)>> v;
   // v.reserve(1);
   // ymLog(VF::UnitTest, "Vector capacity is {}", v.capacity());
   // auto const OldCapacity = v.capacity();
   // for (auto i = 0uz; i < OldCapacity + 1uz; i++)
   // { // force reallocation
   //    v.emplace_back();
   // }

   return {
      {"True", true}
   };
}
