/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#define YM_INTEG_TESTING_ACTIVE_DEFINED

#include "testsuite.h"
#include "ymglobals.h"

// Begin - Structure(s) under test
#include "ymutils.h"
// End - Structure(s) under test

#include "fmt/format.h"

#include <type_traits>

// ----------------------------------------------------------------------------
// Compile-time soundness of the wrapper interface.
// ----------------------------------------------------------------------------

/// @brief bool conversion must be explicit (no implicit decay to arithmetic/bool).
static_assert(!std::is_convertible_v<ym::loose<int>, bool>);
static_assert(!std::is_convertible_v<ym::bound<int>, bool>);

/// @brief nullptr can neither construct nor assign a bound pointer.
static_assert(!std::is_constructible_v<ym::bound<int>, std::nullptr_t>);
static_assert(!std::is_assignable_v <ym::bound<int> &, std::nullptr_t>);

/// @brief Wrappers are pointer-width, trivially copyable values.
static_assert(sizeof(ym::bound<int>) == sizeof(int *));
static_assert(sizeof(ym::loose<int>) == sizeof(int *));
static_assert(std::is_trivially_copyable_v<ym::bound<int>>);
static_assert(std::is_trivially_copyable_v<ym::loose<int>>);

/// @brief Array types are std::span territory - not wrappable.
template <typename T> concept BoundWrappable = requires { typename ym::bound<T>; };
template <typename T> concept LooseWrappable = requires { typename ym::loose<T>; };
static_assert(!BoundWrappable<int[]>);
static_assert(!LooseWrappable<char[]>);
static_assert(BoundWrappable<int>);
static_assert(LooseWrappable<int>);

/// @brief Byte views shall never cast away constness.
static_assert(!std::is_constructible_v<ym::bound<ym::uchar>, ym::str>);
static_assert( std::is_constructible_v<ym::bound<ym::uchar const>, ym::str>);

namespace
{

/**
 * @brief Helper hierarchy for casting tests.
 */
struct Base
{
   virtual ~Base(void) = default;
   virtual auto id(void) const -> int { return 0; }
};

/**
 * @brief Derived type with distinct address and extra member.
 */
struct Derived : public Base
{
   int extra {7};
};

} // anonymous

/**
 * @brief Constructor.
 */
ym::integ::core::pointers::TestSuite::TestSuite(void) :
   TestSuiteBase("Pointer_Handling")
{
   addTestCase<InteractiveInspection>();
   addTestCase<SmokeTest>();
   addTestCase<Soundness_BoundConstruction>();
   addTestCase<Soundness_BoundComparisons>();
   addTestCase<Soundness_LooseNullness>();
   addTestCase<Soundness_LooseCheckedAccess>();
   addTestCase<Soundness_ComparisonIsPointerBased>();
   addTestCase<Soundness_Casting>();
}

/**
 * @brief Interactive inspection - for debug purposes.
 *
 * @param InData -- Important values for test.
 *
 * @returns DataShuttle -- Important values acquired during test.
 */
auto ym::integ::core::pointers::TestSuite::InteractiveInspection::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   return {};
}

/**
 * @brief Basic integrity test.
 *
 * @param InData -- Important values for test.
 *
 * @returns DataShuttle -- Important values acquired during test.
 */
auto ym::integ::core::pointers::TestSuite::SmokeTest::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   int val {123};

   ym::bound bp {&val};
   ym::loose lp {&val};

   return {
      {"BoundDeref",    *bp == 123},
      {"LooseDeref",    *lp == 123},
      {"BoundGetRaw",   bp.get() == &val},
      {"LooseGetRaw",   lp.get() == &val},
   };
}

/**
 * @brief bound<T> construction invariants: non-null enforcement and the
 *        assume-non-null bypass.
 *
 * @param InData -- Important values for test.
 *
 * @returns DataShuttle -- Important values acquired during test.
 */
auto ym::integ::core::pointers::TestSuite::Soundness_BoundConstruction::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   int const val {42};
   int * const raw = const_cast<int *>(&val);

   ym::bound<int> bp {raw};
   bool const from_raw_ok = (bp.get() == raw) && (*bp == 42);

   ym::bound<int> tagged {raw, ym::ym_AssumePtrNotNull{}};
   bool const tagged_ok = (tagged.get() == raw);

   bool const null_cmp_is_false = (bp == nullptr) == false;
   bool const bool_true         = static_cast<bool>(bp);

   bool null_rejected = true;

   #if (YM_EXCEPTIONS_ENABLED)
      int * np {nullptr};
      null_rejected = false;
      try {
         ym::bound<int> bad {np};
         static_cast<void>(bad);
      }
      catch (ym::ym_NullPtrError const &) {
         null_rejected = true;
      }
   #else
      // With exceptions disabled a null construction aborts by design
      // (cannot be caught) - the compile-time rejections are covered by the
      // static_asserts at the top of this file.
   #endif

   return {
      {"FromRawOk",          from_raw_ok},
      {"TaggedOk",           tagged_ok},
      {"NullCmpIsFalse",     null_cmp_is_false},
      {"BoolTrue",           bool_true},
      {"NullRejected",       null_rejected},
   };
}

/**
 * @brief bound<T> comparisons compare stored pointers with a total order.
 *
 * @param InData -- Important values for test.
 *
 * @returns DataShuttle -- Important values acquired during test.
 */
auto ym::integ::core::pointers::TestSuite::Soundness_BoundComparisons::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   int a {};
   int b {};

   ym::bound<int> ba  {&a};
   ym::bound<int> bb  {&b};
   ym::bound<int> ba2 {&a};

   bool const same_addr_equal = (ba == ba2);
   bool const diff_addr_neq   = (ba != bb);
   bool const cmp_consistent  = (ba == ba2) && (ba != bb) && ((ba < bb) != (bb < ba));
   bool const irreflexive     = !(ba < ba2) && !(ba2 < ba);
   bool const total_order     = !((ba < bb) && (bb < ba)) && ((ba < bb) || (ba == bb) || (ba > bb));

   return {
      {"SameAddrEqual", same_addr_equal},
      {"DiffAddrNeq",   diff_addr_neq},
      {"CmpConsistent", cmp_consistent},
      {"Irreflexive",   irreflexive},
      {"TotalOrder",    total_order},
   };
}

/**
 * @brief loose<T> nullness handling: default-null, reset, unwrap, unwrap_or.
 *
 * @param InData -- Important values for test.
 *
 * @returns DataShuttle -- Important values acquired during test.
 */
auto ym::integ::core::pointers::TestSuite::Soundness_LooseNullness::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   int val {9};

   ym::loose<int> lp {};
   bool const default_null = (lp == nullptr) && !static_cast<bool>(lp);

   lp.reset(&val);
   bool const reset_set = static_cast<bool>(lp) && (lp.get() == &val);

   bool const unwrap_ok        = (lp.unwrap().get() == &val) && (*lp.unwrap() == 9);
   bool const unwrap_or_value  = (*lp.unwrap_or(ym::bound<int> {&val})) == 9;

   lp.reset();
   bool const reset_null = (lp == nullptr);
   bool const unwrap_or_fallback = (*lp.unwrap_or(ym::bound<int> {&val})) == 9;

   bool unwrap_throws = true;

   #if (YM_EXCEPTIONS_ENABLED)
      unwrap_throws = false;
      try {
         static_cast<void>(lp.unwrap());
      }
      catch (ym::ym_NullPtrError const &) {
         unwrap_throws = true;
      }
   #endif

   return {
      {"DefaultNull",       default_null},
      {"ResetSet",          reset_set},
      {"ResetNull",         reset_null},
      {"UnwrapOk",          unwrap_ok},
      {"UnwrapThrows",      unwrap_throws},
      {"UnwrapOrValue",     unwrap_or_value},
      {"UnwrapOrFallback",  unwrap_or_fallback},
   };
}

/**
 * @brief loose<T> checked access: dereference/arrow throw on null; invoke
 *        propagates results and reports void invocation.
 *
 * @param InData -- Important values for test.
 *
 * @returns DataShuttle -- Important values acquired during test.
 */
auto ym::integ::core::pointers::TestSuite::Soundness_LooseCheckedAccess::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   int val {5};

   ym::loose<int> lp {&val};
   bool const deref_ok = (*lp == 5);

   *lp = 6;
   bool const write_ok = (val == 6);

   auto const opt      = lp.invoke([](int * const p) { return *p * 10; });
   bool const invoke_value = opt.has_value() && (*opt == 60);

   bool side_affected = false;
   bool const invoke_void_ran = lp.invoke([&side_affected](int *) { side_affected = true; });
   bool const invoke_side     = side_affected;

   ym::loose<int> ln {};
   bool const invoke_null_nullopt = !ln.invoke([](int * const p) { return *p; }).has_value();
   bool const invoke_null_void    = !ln.invoke([](int *) {});

   bool deref_throws = true;
   bool arrow_throws = true;

   #if (YM_EXCEPTIONS_ENABLED)
      deref_throws = false;
      arrow_throws = false;
      try {
         static_cast<void>(*ln);
      }
      catch (ym::ym_NullPtrError const &) {
         deref_throws = true;
      }

      struct Holder { int v {3}; };
      Holder h;
      ym::loose<Holder> hl {};
      try {
         static_cast<void>(hl->v);
      }
      catch (ym::ym_NullPtrError const &) {
         arrow_throws = true;
      }
   #endif

   return {
      {"DerefOk",           deref_ok},
      {"WriteOk",           write_ok},
      {"InvokeValue",       invoke_value},
      {"InvokeVoidRan",     invoke_void_ran},
      {"InvokeSide",        invoke_side},
      {"InvokeNullNullopt", invoke_null_nullopt},
      {"InvokeNullVoid",    invoke_null_void},
      {"DerefThrows",       deref_throws},
      {"ArrowThrows",       arrow_throws},
   };
}

/**
 * @brief Regression guard: comparisons between wrappers must observe the
 *        stored address - never nullness (a historical defect compared
 *        wrappers by their bool conversion, making all non-null wrappers
 *        equal).
 *
 * @param InData -- Important values for test.
 *
 * @returns DataShuttle -- Important values acquired during test.
 */
auto ym::integ::core::pointers::TestSuite::Soundness_ComparisonIsPointerBased::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   int a {};
   int b {};

   ym::loose<int> la  {&a};
   ym::loose<int> lb  {&b};
   ym::loose<int> la2 {&a};
   ym::loose<int> n1  {};
   ym::loose<int> n2  {};

   bool const diff_objects_neq  = (la != lb);   // would be EQUAL under bool-compare defect
   bool const same_address_eq   = (la == la2);
   bool const nulls_eq          = (n1 == n2);
   bool const nonnull_neq_null  = (la != n1);
   bool const bound_same_eq     = (ym::bound<int> {&a} == ym::bound<int> {&a});
   bool const bound_diff_neq    = (ym::bound<int> {&a} != ym::bound<int> {&b});

   return {
      {"DiffObjectsNeq",  diff_objects_neq},
      {"SameAddressEq",   same_address_eq},
      {"NullsEq",         nulls_eq},
      {"NonnullNeqNull",  nonnull_neq_null},
      {"BoundSameEq",     bound_same_eq},
      {"BoundDiffNeq",    bound_diff_neq},
   };
}

/**
 * @brief Casting rules: legal casts preserve the address, const-stripping is
 *        rejected (compile-time guarded - see static_asserts above).
 *
 * @param InData -- Important values for test.
 *
 * @returns DataShuttle -- Important values acquired during test.
 */
auto ym::integ::core::pointers::TestSuite::Soundness_Casting::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   Derived d;

   ym::bound<Derived> dp {&d};
   ym::bound<Base>    bp {dp};
   bool const upcast_addr_same = (bp.get() == static_cast<Base *>(&d)) && (bp->id() == 0);

   char buf[8] {'a', 'b', 'c', 'd', 'e', 'f', 'g', '\0'};

   ym::mutstr m {buf};
   ym::bound<ym::uchar> bytes {m};
   bool const byteview_addr = (bytes.get() == reinterpret_cast<ym::uchar *>(buf)) && (bytes[2] == 'c');

   ym::str cs {buf};
   ym::bound<ym::uchar const> cbytes {cs};
   bool const constbyteview_addr = (cbytes.get() == reinterpret_cast<ym::uchar const *>(buf)) && (cbytes[4] == 'e');

   ym::bound<char> pk {dp, ym::ym_PtrCastPassKey{}};
   bool const passkey_addr = (pk.get() == reinterpret_cast<char *>(&d));

   ym::loose<Derived> lp {&d};
   ym::loose<Base>    lbp {lp};
   bool const loose_upcast_same = (lbp.get() == static_cast<Base *>(&d));

   return {
      {"UpcastAddrSame",    upcast_addr_same},
      {"ByteviewAddr",      byteview_addr},
      {"ConstByteviewAddr", constbyteview_addr},
      {"PasskeyAddr",       passkey_addr},
      {"LooseUpcastSame",   loose_upcast_same},
   };
}
