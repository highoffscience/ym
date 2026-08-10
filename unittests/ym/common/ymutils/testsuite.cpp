/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "testsuite.h"
#include "ymglobals.h"

#include "ymutils.h" // Structures under test

#include "fmt/format.h"

#include <string>
#include <sys/stat.h>
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
   addTestCase<PtrIntClass>();
   addTestCase<ByteBitsetClass>();
   addTestCase<BoundedPtrClass>();
   addTestCase<PolyRawClass>();
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
 * @brief Tests global ym functions.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::Funcs::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
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
      fmt::println("empty 1) -- {}", ym_empty(s));

      s = "";
      emptySuccess &= ym_empty(s);
      fmt::println("empty 2) -- {}", ym_empty(s));

      s = "Go! Torchic!";
      emptySuccess &= !ym_empty(s);
      fmt::println("empty 3) -- {}", ym_empty(s));
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
 * @brief Tests PtrInt_T conversion utility structure.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::PtrIntClass::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   int32 vals[] {9, 7};

   PtrInt_T p2i{vals};
   p2i._uint_val += 4u;

   return {
      {"UnionWorks", *p2i._ptr_val == 7}
   };
}

/** run
 *
 * @brief Tests ByteBitset class.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::ByteBitsetClass::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   // Bitset b{};
   // fmt::println("1) TODO --> {}", b.getUnderlying());
   // b.set(0);
   // fmt::println("2) TODO --> {}", b.getUnderlying());
   // b.set(1);
   // fmt::println("3) TODO --> {}", b.getUnderlying());
   // b.clear(1);
   // fmt::println("4) TODO --> {}", b.getUnderlying());
   // b.set(1, true);
   // fmt::println("5) TODO --> {}", b.getUnderlying());
   // auto b2 = b;
   // fmt::println("6) TODO --> {}", b2.getUnderlying());

   return {};
}

/** run
 *
 * @brief TODO
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::BoundedPtrClass::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   // static_assert(std::assignable_from<
   //    BoundPtr<str>&,
   //    BoundPtr<str>
   // >);

   int a = 9;
   int * ap = &a;

   BoundPtr ab(ap);
   *ab = 11;

   [[maybe_unused]] auto ab2 = ab;

   ab = ap;

   int r[3]{};
   BoundPtr rp(r);

   // str fgh = nullptr; // doesn't compile - expected!

   // BoundPtr<int> rp2 = rp; // doesn't compile - expected!

   [[maybe_unused]] optstrlit val = "3";

   [[maybe_unused]] optstrlit val3;
   val3 = "1";

   // optstr val4 = val3; // doesn't compile - expected! (optstrlit -> optstr)

   [[maybe_unused]] strlit val2 = "1";
   val2 = "0";

   // [[maybe_unused]] str val5 = val2; // doesn't compile - expected! (strlit -> str)

   // BoundPtr<void> vb = ab;

   // BoundPtr<int> bb(vb, ym_PtrCastPassKey{});

   auto const Is11 = true; // TODO (*bb == 11);

   [[maybe_unused]] strlit s = "Go! Torchic!";
   // [[maybe_unused]] str s2 = s; // won't compile - good
   // strlit s3 = s2; won't compile - check.

   // struct stat st;
   // if (stat(s2, &st) == 0)
   // {

   // }

   [[maybe_unused]] str n = "hghg";
   // strlit m = n; // doesn't compile - expected!



   str v = s; // str array to string pointer - expected to compile but only with byte size types
   [[maybe_unused]] optstr v2 = s;
   [[maybe_unused]] optstr u = v;
   [[maybe_unused]] optstr w = s.get();

   [[maybe_unused]]
   rawstr const Argv[] = {"testsuite",
      "--input",  "settings.json",
      "--output", "data.csv",
      "-cb",
      "-k", "Torchic1234",
      "--in-denial"
   };
   [[maybe_unused]]
   rawstr const * Argv_a = Argv;
   [[maybe_unused]]
   BoundPtr<rawstr const> const Argv_BPtr = Argv;

   return {
      {"CanCast", Is11}
   };
}

/** run
 *
 * @brief TODO
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::TestSuite::PolyRawClass::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
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
   // fmt::println("Vector capacity is {}", v.capacity());
   // auto const OldCapacity = v.capacity();
   // for (auto i = 0uz; i < OldCapacity + 1uz; i++)
   // { // force reallocation
   //    v.emplace_back();
   // }

   return {
      {"True", true}
   };
}
