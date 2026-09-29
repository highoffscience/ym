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

/**
 * @brief Constructor.
 */
ym::unit::ymutils::TestSuite::TestSuite(void) :
   TestSuiteBase("YmUtils")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
   addTestCase<Func_castPtrTo>();
   addTestCase<Func_binarySearch>();
   addTestCase<Class_PtrInt>();
   addTestCase<Class_ByteBitset>();
   addTestCase<Class_BoundPtr>();
   addTestCase<Class_LoosePtr>();
   addTestCase<Class_PolyRaw>();
}

/**
 * @brief Interactive inspection - for debug purposes.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::ymutils::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}

/**
 * @brief Basic integrity test.
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::ymutils::TestSuite::SmokeTest::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {
      {"True", true}
   };
}

/**
 * - @ref ym::ym_castPtrTo() Shall ?
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::ymutils::TestSuite::Func_castPtrTo::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
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

   return {
      {"castPtrToSuccess", castPtrToSuccess}
   };
}

/**
 * - @ref ym::ym_binarySearch() Shall ?
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::ymutils::TestSuite::Func_castPtrTo::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto binarySearchSuccess = false;
   {
      std::vector v = {1,2,3,4,5,6,7,8,9};
      auto e = ym_binarySearch(v.cbegin(), v.cend(), 5);
      binarySearchSuccess = (*e == 5);

      e = ym_binarySearch(v.cbegin(), v.cend(), 10);
      binarySearchSuccess &= (e == v.cend());
   }

   return {
      {"binarySearchSuccess", binarySearchSuccess}
   };
}

/**
 * - @ref ym::PtrInt_T Shall ?
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::ymutils::TestSuite::Class_PtrInt::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   int32 vals[] {9, 7};

   PtrInt_T p2i{vals};
   p2i._uint_val += 4u;

   return {
      {"UnionWorks", *p2i._ptr_val == 7}
   };
}

/**
 * - @ref ym::ByteBitset Shall ?
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::ymutils::TestSuite::Class_ByteBitset::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {
      {"True", true}
   };
}

/**
 * - @ref ym::BoundPtr Shall ?
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::ymutils::TestSuite::Class_BoundPtr::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
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

/**
 * - @ref ym::LoosePtr Shall ?
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::ymutils::TestSuite::Class_LoosePtr::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {
      {"True", true}
   };
}

/**
 * - @ref ym::PolyRaw Shall ?
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::unit::ymutils::TestSuite::Class_PolyRaw::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
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
