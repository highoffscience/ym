/**
 * @file    ymutils_pointer_brainstorm.h
 * @version 0.2.0
 * @author  Forrest Jablonski
 *
 * @brief   EXPERIMENTAL - make ym::bound / ym::loose fancy pointers that std smart
 *          pointers store and speak through. Not included by ymutils.h, not wired
 *          into any build target.
 *
 * This revision replaces the "owner borrow" draft (bound<std::unique_ptr<T>> and
 * friends). The inversion is the point: instead of ym types wrapping std owners,
 * STD OWNERS STORE YM TYPES. std keeps ownership (control blocks, deleters, move
 * semantics - all std's job, no ym interface to duplicate); the null discipline
 * rides INSIDE the pointer member of the smart pointer itself. The stored member
 * is a bound<T>/loose<T>, and since those wrappers are raw-T*-sized and raw-T*-
 * laid-out, "the smart pointer holds the raw pointer, everything goes through
 * the fancy pointer type" is literally true: one pointer of storage, zero added
 * indirection, checks in the type.
 *
 * ----------------------------------------------------------------------------
 * THE THREE INTEGRATION SURFACES (who accepts a custom pointer, and how)
 * ----------------------------------------------------------------------------
 *
 * 1. std::unique_ptr - THE documented hook: the "pointer" typedef in the Deleter
 *    (P1902R2, adopted C++23). If Deleter::pointer exists it becomes
 *    unique_ptr::pointer AND the stored member type; construction, get(),
 *    release() and reset() all speak it instead of T*. There is no feature test
 *    macro - @ref ym_StdUniqueAdoptsPointer probes the platform directly.
 *
 * 2. std::shared_ptr - NO deleter-side hook exists: shared_ptr::pointer is always
 *    element_type*, get() always returns raw. The only surface that can carry a
 *    custom pointer is the ALLOCATOR of std::allocate_shared: allocator_traits
 *    propagates A::pointer through the whole control-block lifecycle (allocate,
 *    the allocated_ptr guard - which calls pointer_traits<pointer>::pointer_to -,
 *    construct, destroy, deallocate). Committee support for allocate_shared is
 *    still in flight, so treat fancy shared_ptr as IMPLEMENTATION-BEHAVIOR, not
 *    std contract: verified working end to end on libstdc++ 16 in both C++23 and
 *    C++26 modes (see support matrix below). The public shared_ptr interface
 *    stays raw no matter what is stored, so ym's checks live at the factory door
 *    (construction) and at the @ref ym_asBound/@ref ym_asLoose exits.
 *
 * 3. std::weak_ptr - has no pointer surface of its own; it inherits the owner's
 *    through lock(). The expiry check IS the null check: ym_asBound(weak) throws
 *    on expired, ym_asLoose(weak) reports null.
 *
 * ----------------------------------------------------------------------------
 * WHICH YM TYPE PLACES WHERE (the empirical answer)
 * ----------------------------------------------------------------------------
 *
 * loose<T> is the STORAGE type. unique_ptr's member is genuinely nullable -
 * moved-from owners, release() and reset() all seat null into it - which is
 * exactly loose's domain, and loose satisfies every requirement std implementations
 * put on a stored pointer as of today, unmodified:
 *    - default constructible to null                     (Ptr_Base member init)
 *    - implicit from T*                                  (ctor (2))
 *    - pairwise ==/!=, == nullptr, explicit bool         (Ptr_Base + unwrap path)
 *    - *p and p-> usable by std internals                (checked - a null leak
 *                                                         into std's guts becomes
 *                                                         a reported error)
 *    - pointer_traits auto-derives element_type/rebind,  (verified: only
 *      to_address() recurses operator-> to raw            pointer_to was missing)
 *
 * bound<T> is the POLICY type, present at every boundary where a real non-null
 * guarantee exists:
 *    - the Deleter parameter (std guarantees the deleter runs only on non-null)
 *    - factory construction (new never returns null; the check is for showings-up
 *      from elsewhere)
 *    - ym_asBound() accessors (checked views out of raw-typed std interfaces)
 * bound<T> CANNOT be a stored member under today's ymutils.h: std implementations
 * evaluate pointer() (a null-constructed pointer) inside ~unique_ptr, operator*,
 * move assignment and the shared machinery, and BoundPtr has no default
 * constructor. That is not a quirk to fight - a default-constructible bound would
 * silently admit null bounds through std's own reset()/default-construct, hollow
 * out the ymutils.h invariant, and the friendship route cannot help (std calls
 * pointer() from non-friend code). If it is ever wanted anyway, the only shape
 * is a quiet defaulted BoundPtr() in ymutils.h - §OPTION C below.
 *
 * ----------------------------------------------------------------------------
 * EMPIRICAL SUPPORT MATRIX (probes run: libstdc++ 16.1, C++23 and C++26 modes)
 * ----------------------------------------------------------------------------
 *
 *    unique_ptr<T, Deleter{pointer=loose<T>}>            works fully (P1902R2)
 *    unique_ptr get() returns loose<T>                   yes (P1902R2)
 *    unique_ptr<T, Deleter{no typedef, param=bound<T>}>  works fully (raw storage)
 *    unique_ptr stores bound<T>                          NO - pointer() required
 *    pointer_traits element_type/rebind/to_address       auto-conformant already
 *    pointer_traits pointer_to                           MISSING - supplied below
 *    allocate_shared, A::pointer=loose<T>                works fully (impl detail)
 *    weak_ptr lock/expires with fancy owner              works fully
 *    sizeof owners                                       unchanged (1/2 pointers) -
 *                                                        requires the deleters to
 *                                                        INHERIT their policy (EBC;
 *                                                        an empty data member
 *                                                        member still bloats to 16)
 *
 * ----------------------------------------------------------------------------
 * INTEGRATION NOTE (matters): the pointer_traits specializations below are the
 * ONLY glue, and they are glue ymutils.h could own outright by adding a
 * `static pointer pointer_to(T&)` to each wrapper (then these specializations
 * get deleted - the primaries auto-derive everything else). Everything else in
 * this file is additive and ODR-safe the moment the header is included everywhere
 * the aliases are used, same caveat as any header-only vocabulary type.
 */

#pragma once

#include "ymutils.h"

namespace ym
{

// ----------------------------------------------------------------------------
// Conformance probes - what THIS platform requires, and what we satisfy.
// ----------------------------------------------------------------------------

/**
 * @brief Mirrors the requirements std implementations (as verified on libstdc++
 *        16) actually place on a pointer stored inside a smart pointer. This is
 *        a superset of the old NullablePointer wording: default construction to
 *        null and pairwise comparison are load-bearing, not stylistic.
 *
 * @tparam Ptr_T -- Candidate stored pointer type.
 */
template <typename Ptr_T>
concept ym_StdFancyPointer =
    std::default_initializable<Ptr_T> &&
    std::equality_comparable<Ptr_T> &&
    requires (typename std::pointer_traits<Ptr_T>::element_type & r, Ptr_T p) {
       { std::to_address(p) };                                    // must reach raw
       { std::pointer_traits<Ptr_T>::pointer_to(r) } -> std::same_as<Ptr_T>;
       { static_cast<bool>(p) } -> std::same_as<bool>;
       { *p };                                                    // std internals deref
    } &&
    std::convertible_to<typename std::pointer_traits<Ptr_T>::element_type *, Ptr_T>;

/// @cond INTERNAL
template <
   typename T,
   typename D,
   typename = void>
struct ym_StdUniqueAdoptsPointerImpl : std::false_type { }; // no typedef -> raw storage, not an adoption

template <typename T, typename D>
struct ym_StdUniqueAdoptsPointerImpl<T, D, std::void_t<typename D::pointer>>
   : std::bool_constant<std::is_same_v<typename std::unique_ptr<T, D>::pointer, typename D::pointer>> { };
/// @endcond

/**
 * @brief True when this platform's std::unique_ptr honours the Deleter pointer
 *        typedef (P1902R2). The committee shipped no feature test macro, so
 *        detect the behavior, not the version number. Deleters without a pointer
 *        typedef are well-formed here and simply report false.
 *
 * @tparam T -- Pointee type.
 * @tparam D -- Deleter type.
 */
template <
   typename T,
   typename D>
inline constexpr bool ym_StdUniqueAdoptsPointer = ym_StdUniqueAdoptsPointerImpl<T, D>::value;

/**
 * @brief A deleter policy is usable when it is callable with the fancy pointer
 *        it wants to see. No pointer typedef required - without one the member
 *        stays a raw T* and the fancy type polices the call boundary only.
 *
 * @tparam D     -- Deleter type.
 * @tparam Ptr_T -- Whatever pointer type unique_ptr ends up handing it.
 */
template <
   typename D,
   typename Ptr_T>
concept ym_FancyDeleter = std::invocable<D &, Ptr_T>;

} // ym

// ----------------------------------------------------------------------------
// pointer_traits glue - the one capability the primaries do not yet carry.
// ----------------------------------------------------------------------------

/**
 * @brief Supplies pointer_to for the wrappers (allocate_shared's internal
 *        allocated_ptr guard calls it; element_type/rebind/to_address already
 *        auto-derive from the primaries and are mirrored here verbatim).
 *
 * - Promotion path: add
 *       static constexpr BoundPtr<T> pointer_to(T & r) noexcept { return BoundPtr<T>(std::addressof(r)); }
 *   to BoundPtr (and the LoosePtr equivalent) in ymutils.h and DELETE these
 *   specializations - pointer_traits will then find the members by itself.
 */
template <typename T>
struct std::pointer_traits<ym::BoundPtr<T>>
{
   using pointer         = ym::BoundPtr<T>;
   using element_type    = T;
   using difference_type = std::ptrdiff_t; // specializations must spell it - the primary's fallback does not apply

   template <typename U>
   using rebind = ym::BoundPtr<U>;

   /// @brief Non-null by construction - taking a reference makes this the free case.
   static pointer pointer_to(T & r) noexcept { return pointer(std::addressof(r)); }

   /// @brief The wrapper is a raw pointer; the address is the address.
   static constexpr T * to_address(const pointer & p) noexcept { return p.get(); }
};

template <typename T>
struct std::pointer_traits<ym::LoosePtr<T>>
{
   using pointer         = ym::LoosePtr<T>;
   using element_type    = T;
   using difference_type = std::ptrdiff_t; // specializations must spell it - the primary's fallback does not apply

   template <typename U>
   using rebind = ym::LoosePtr<U>;

   /// @brief May legitimately produce a null pointer value (loose admits null).
   static pointer pointer_to(T & r) noexcept { return pointer(std::addressof(r)); }

   static constexpr T * to_address(const pointer & p) noexcept { return p.get(); }
};

namespace ym
{

// ----------------------------------------------------------------------------
// Deleters - where the fancy pointer types express their guarantees.
// ----------------------------------------------------------------------------

/**
 * @brief Fancy-storage deleter: declares pointer = loose<T>, so the unique_ptr
 *        member IS a loose<T>; forwards deletion to Base_T on the raw address.
 *
 * - std guarantees the deleter runs only on a non-null stored pointer, so
 *   p.get() cannot be null on the destruction path.
 *
 * @tparam T      -- Pointee type.
 * @tparam Base_T -- Underlying deletion policy (must accept T*).
 */
template <
   typename T,
   typename Base_T = std::default_delete<T>>
requires (ym_Pointable<T> and std::invocable<Base_T &, T *>)
// Inherit the policy, never hold it: even an empty data member makes the deleter
// non-empty and bloats the owner to 16 bytes (measured); inheritance keeps a
// stateless policy free via EBC and a stateful one honestly sized.
class LooseDelete : private Base_T
{
public:
   using pointer = loose<T>;

   /**
    * @brief Deletes the managed object via the base policy.
    *
    * @throws std::exception -- Whatever Base_T throws (none for default_delete).
    *
    * @param p -- The fancy stored pointer handed over by the owning unique_ptr.
    */
   void operator() (pointer p) const noexcept(std::is_nothrow_invocable_v<Base_T &, T *>) {
      Base_T::operator()(p.get());
   }

   /// @brief Access to the wrapped policy (std::unique_ptr::get_deleter parity).
   constexpr Base_T & deleter(void) noexcept { return *this; }
   constexpr Base_T const & deleter(void) const noexcept { return *this; }
};

/**
 * @brief Policy-only deleter: deliberately NO pointer typedef, so the unique_ptr
 *        member stays a literal raw T*, and the deleter parameter bound<T> forces
 *        the non-null check at the one moment it is semantically meaningful -
 *        the moment of deletion. The most literal reading of "holds the raw
 *        pointer, everything goes through the fancy pointer type".
 *
 * - Also the pre-P1902 fallback: works on any C++ standard, any implementation.
 *
 * @tparam T      -- Pointee type.
 * @tparam Base_T -- Underlying deletion policy (must accept T*).
 */
template <
   typename T,
   typename Base_T = std::default_delete<T>>
requires (ym_Pointable<T> and std::invocable<Base_T &, T *>)
class BoundPolicyDelete : private Base_T // inherit, never hold - see LooseDelete layout note
{
public:
   /**
    * @{
    * @brief Deletes via the base policy. The parameter type is the statement:
    *        reaching deletion with null is a reported error, not UB.
    *
    * @throws ym_NullPtrError -- If handed null (std never does; user get_deleter()
    *         calls can).
    * @throws std::exception  -- Whatever Base_T throws.
    *
    * @param p -- Pointer handed over by the owning unique_ptr.
    */
   void operator() (bound<T> p) const { Base_T::operator()(p.get()); }
   /// @}

   constexpr Base_T & deleter(void) noexcept { return *this; }
   constexpr Base_T const & deleter(void) const noexcept { return *this; }
};

// ----------------------------------------------------------------------------
// Allocator - the shared_ptr/weak_ptr entry point.
// ----------------------------------------------------------------------------

/**
 * @brief Allocator whose pointer typedef is loose<T>. std::allocate_shared
 *        propagates it through the control-block lifecycle: allocation, the
 *        pointer_to guard, in-place construction/destruction, deallocation.
 *        Rebinding works because the template's first parameter is the pointee
 *        (allocator_traits substitutes it), which is also why loose<T> must keep
 *        element type as its sole pointer template argument.
 *
 * @tparam T -- Pointee type.
 */
template <typename T>
requires ym_Pointable<T>
class LooseAllocator
{
public:
   using value_type = T;
   using pointer    = loose<T>;

   /// @brief Rebinding ctor - every control-block node rebinds from LooseAllocator<T>.
   template <typename U> constexpr LooseAllocator(LooseAllocator<U> const &) noexcept { }
   constexpr LooseAllocator(void) noexcept = default;

   /**
    * @brief Allocates raw; the implicit raw->loose conversion at the call site is
    *        the first fancy crossing.
    *
    * @throws std::bad_alloc -- From the base allocator.
    *
    * @param n -- Number of elements.
    *
    * @returns pointer -- Storage for n elements.
    */
   T * allocate(std::size_t const n) { return _back.allocate(n); }

   /**
    * @brief Deallocates through the fancy pointer (unwrapped at the raw boundary).
    *
    * @param p -- Storage to free.
    * @param n -- Number of elements.
    */
   void deallocate(pointer const & p, std::size_t const n) noexcept { _back.deallocate(p.get(), n); }

   /// @{
   /// @brief Construct/destroy the object living AT a fancy slot. Deref of the
   ///        slot goes through loose's checked path.
   /// @throws std::exception -- From the constructed object's ctor.
   template <
      typename    U,
      typename... Args_T>
   void construct(loose<U> * const slot_Ptr, Args_T &&... args) {
      std::construct_at(std::addressof(*slot_Ptr), std::forward<Args_T>(args)...);
   }

   template <typename U>
   void destroy(loose<U> * const slot_Ptr) noexcept(std::is_nothrow_destructible_v<U>) {
      std::destroy_at(std::addressof(*slot_Ptr));
   }
   /// @}

   template <typename U>
   friend bool operator == (LooseAllocator const &, LooseAllocator<U> const &) noexcept { return true; }
   template <typename U>
   friend bool operator != (LooseAllocator const &, LooseAllocator<U> const &) noexcept { return false; }

private:
   std::allocator<T> _back {};
};

// ----------------------------------------------------------------------------
// Vocabulary: owner aliases + checked factories.
// ----------------------------------------------------------------------------

/**
 * @brief Exclusive owner whose stored pointer IS a loose<T>: null discipline in
 *        the member, checked deref via loose's unwrap path, get()/release()/
 *        reset() speaking loose.
 */
template <
   typename T,
   typename Base_T = std::default_delete<T>>
using loose_unique = std::unique_ptr<T, LooseDelete<T, Base_T>>;

/**
 * @brief Exclusive owner with literal raw-pointer storage that polices deletion
 *        through bound<T> (policy-only deleter). The portable variant.
 */
template <
   typename T,
   typename Base_T = std::default_delete<T>>
using checked_unique = std::unique_ptr<T, BoundPolicyDelete<T, Base_T>>;

/// @{
/**
 * @brief Factories. new cannot return null (bad_alloc), so the bound/loose
 *        conversions cannot throw here; they exist to keep every crossing
 *        explicit at the call graph boundary.
 *
 * @throws std::exception -- From the constructed T.
 *
 * @tparam T      -- Pointee type.
 * @tparam Args_T -- Deduced ctor argument pack.
 *
 * @param args -- Arguments forwarded to T's constructor.
 *
 * @returns auto -- The requested owner.
 */
template <
   typename T,
   typename... Args_T>
requires ym_Pointable<T>
auto ym_makeLooseUnique(Args_T &&... args) {
   return loose_unique<T>(new T(std::forward<Args_T>(args)...));
}

template <
   typename T,
   typename... Args_T>
requires ym_Pointable<T>
auto ym_makeCheckedUnique(Args_T &&... args) {
   return checked_unique<T>(new T(std::forward<Args_T>(args)...));
}

/**
 * @brief Shared owner backed by the fancy allocator: the control block stores
 *        loose<T> end to end (libstdc++-verified behavior, see header).
 *
 * @throws std::exception -- From allocation or T's constructor.
 *
 * @tparam T      -- Pointee type.
 * @tparam Args_T -- Deduced ctor argument pack.
 *
 * @param args -- Arguments forwarded to T's constructor.
 *
 * @returns auto -- shared_ptr with loose-backed control block.
 */
template <
   typename T,
   typename... Args_T>
requires ym_Pointable<T>
auto ym_allocateFancyShared(Args_T &&... args) {
   return std::allocate_shared<T>(LooseAllocator<T>{}, std::forward<Args_T>(args)...);
}
/// @}

// ----------------------------------------------------------------------------
// Exits: checked/nullable views out of raw-typed std interfaces.
// ----------------------------------------------------------------------------

/// @{
/**
 * @brief Projects any smart pointer to the ym family. to_address normalizes
 *        raw members, loose members (P1902 unique_ptr) and bound members alike,
 *        so these work for all three owners regardless of what is stored.
 *
 * @tparam T -- (Deduced) pointee.
 *
 * @param owner -- Smart pointer to project.
 *
 * @returns auto -- @ref ym::loose (never throws) or @ref ym::bound.
 *
 * @throws ym_NullPtrError -- ym_asBound on an empty owner: a moved-from/released
 *         unique_ptr, or an expired weak_ptr. Both are misuses worth reporting.
 */
template <typename T, typename D>
requires ym_Pointable<T>
constexpr auto ym_asLoose(std::unique_ptr<T, D> const & owner) noexcept {
   return loose<T>(std::to_address(owner.get()));
}

template <typename T, typename D>
requires ym_Pointable<T>
auto ym_asBound(std::unique_ptr<T, D> const & owner) {
   return bound<T>(std::to_address(owner.get()));
}

template <typename T>
requires ym_Pointable<T>
constexpr auto ym_asLoose(std::shared_ptr<T> const & owner) noexcept {
   return loose<T>(owner.get());
}

template <typename T>
requires ym_Pointable<T>
auto ym_asBound(std::shared_ptr<T> const & owner) {
   return bound<T>(owner.get());
}

template <typename T>
requires ym_Pointable<T>
auto ym_asLoose(std::weak_ptr<T> const & owner) noexcept {
   return loose<T>(owner.lock().get()); // lock() temp keeps the target alive long enough to capture
}

template <typename T>
requires ym_Pointable<T>
auto ym_asBound(std::weak_ptr<T> const & owner) {
   return bound<T>(owner.lock().get());
}
/// @}

// ----------------------------------------------------------------------------
// §OPTION C - bound<T> as the stored member (NOT recommended; kept visible).
// ----------------------------------------------------------------------------
/*
 * If ymutils.h ever grows a quiet defaulted BoundPtr() (== silently nullable
 * bound), the machinery below lights up with zero further changes. It is gated
 * on the concept, not a compiler version: ym_StdFancyPointer<bound<T>> is false
 * today (no default ctor), and everything in this section degrades cleanly.
 *
 * Rejected because std's own default-construct/reset()/moved-from paths would
 * manufacture null bounds the type promises cannot exist - the invariant would
 * become "checked at some constructions" which is just loose with extra steps.
 */
template <
   typename T,
   typename Base_T = std::default_delete<T>>
requires (ym_Pointable<T> and ym_StdFancyPointer<bound<T>> and std::invocable<Base_T &, T *>)
class BoundStoreDelete : private Base_T
{
public:
   using pointer = bound<T>; // requires ym_StdFancyPointer<bound<T>> - false until ymutils.h opts in

   void operator() (pointer p) const { Base_T::operator()(p.get()); }
};

template <
   typename T,
   typename Base_T = std::default_delete<T>>
requires (ym_Pointable<T> and ym_StdFancyPointer<bound<T>>)
using bound_unique = std::unique_ptr<T, BoundStoreDelete<T, Base_T>>;

} // ym

// ----------------------------------------------------------------------------
// Interface soundness pins.
// ----------------------------------------------------------------------------

namespace ym
{

/// @brief pointer_traits: element_type/rebind/to_address auto-conform without help;
///        pointer_to works now that the specialization exists.
static_assert(std::is_same_v<std::pointer_traits<loose<int>>::element_type, int>);
static_assert(std::is_same_v<std::pointer_traits<bound<int>>::element_type, int>);
static_assert(std::is_same_v<typename std::pointer_traits<loose<int>>::rebind<double>, loose<double>>);
static_assert(std::is_same_v<typename std::pointer_traits<bound<int>>::rebind<double>, bound<double>>);
extern int ym_pin_target; // declared, never defined - its address is a constant expression, no object needed
static_assert(std::is_same_v<decltype(std::pointer_traits<loose<int>>::pointer_to(ym_pin_target)), loose<int>>);
static_assert(std::is_same_v<decltype(std::pointer_traits<bound<int>>::pointer_to(ym_pin_target)), bound<int>>);
static_assert(std::to_address(bound<int>(&ym_pin_target)) == &ym_pin_target);
static_assert(std::to_address(loose<int>(&ym_pin_target)) == &ym_pin_target);

/// @brief Role assignment: loose can live in std's storage; bound cannot (yet) -
///        the negative pin IS the design rationale, and it flips automatically if
///        ymutils.h ever adds a default ctor.
static_assert(ym_StdFancyPointer<loose<int>>);
static_assert(!ym_StdFancyPointer<bound<int>>);
static_assert(ym_StdFancyPointer<int *>); // raw pointers obviously qualify too

/// @brief Deleter policies are callable with what they ask for. A single bound
///        parameter keeps raw->fancy selection unambiguous.
static_assert(std::invocable<LooseDelete<int> &, loose<int>>);
static_assert(std::invocable<BoundPolicyDelete<int> &, int *>);   // raw member, implicit bound param

/// @brief Storage identity: the owner is still exactly one pointer wide.
static_assert(sizeof(loose_unique<int>)          == sizeof(void *));
static_assert(sizeof(checked_unique<int>)        == sizeof(void *));
static_assert(sizeof(std::shared_ptr<int>)       == 2 * sizeof(void *));
static_assert(sizeof(std::weak_ptr<int>)         == 2 * sizeof(void *));

/// @brief P1902 uptake (hard-pinned only where verified).
#if defined(YM_GNU_COMPILER_DEFINED)
   static_assert(ym_StdUniqueAdoptsPointer<int, LooseDelete<int>>);
   static_assert(std::is_same_v<loose_unique<int>::pointer, loose<int>>);
   static_assert(std::is_same_v<decltype(std::declval<loose_unique<int> const &>().get()), loose<int>>);
   static_assert(!ym_StdUniqueAdoptsPointer<int, BoundPolicyDelete<int>>); // no typedef -> stays raw
   static_assert(std::is_same_v<checked_unique<int>::pointer, int *>);
#endif

/// @brief The exits return the ym family (ym_Pointable keeps arrays out of the family itself).
static_assert(std::is_same_v<decltype(ym_asLoose(std::declval<loose_unique<int> const &>())), loose<int>>);
static_assert(std::is_same_v<decltype(ym_asBound(std::declval<loose_unique<int> const &>())), bound<int>>);
static_assert(std::is_same_v<decltype(ym_asBound(std::declval<std::shared_ptr<int> const &>())), bound<int>>);
static_assert(std::is_same_v<decltype(ym_asLoose(std::declval<std::weak_ptr<int const> const &>())), loose<int const>>);
static_assert(std::is_same_v<decltype(ym_asLoose(std::declval<std::unique_ptr<int> const &>())), loose<int>>); // plain std owners project too

} // ym

/*
 * Typical integration - existing code keeps its std members, gains checked
 * boundaries, and pays nothing for it in size:
 *
 * @code
 *    struct Registry {
 *       std::shared_ptr<Device> sp_dev;
 *       auto device(void) { return ym::ym_asBound(sp_dev); }                   // throws if empty
 *       ym::loose_unique<Device> owned = ym::ym_makeLooseUnique<Device>(...);  // member IS a loose<Device>
 *       std::shared_ptr<Device> shared = ym::ym_allocateFancyShared<Device>(...); // control block stores loose<Device>
 *    };
 *
 *    void use(ym::loose_unique<Device> const & dev) {
 *       ym::ym_asBound(dev)->open();   // one throw site, checked, vs. UB
 *    }
 * @endcode
 *
 * Open questions for the next revision:
 *    - Promote pointer_to into ymutils.h primaries and delete the specializations?
 *      (Member route is cleaner; specialization route keeps this file standalone.)
 *    - loose_unique::operator* currently pays loose's unwrap check twice over
 *      (unique_ptr's own contract assert + loose::unwrap). Measure, then decide
 *      whether a no-check storage tier (ym::raw_like<T>) earns its keep.
 *    - Should ym_asLoose(weak) hand back a loose that outlives the lock() temp?
 *      Today it captures the raw target - same caveat as storing a raw from
 *      lock().get(); a strong_ref-holding accessor may be the honest answer.
 *    - PMR interplay: LooseAllocator over std::allocator is hand-rolled; an
 *      allocator_adapter over std::pmr::polymorphic_allocator reuses its equality
 *      and resource plumbing - worth testing next round.
 */
