/**
 * @file    ymutils_pointer_brainstorm.h
 * @version 0.1.0
 * @author  Forrest Jablonski
 *
 * @brief   EXPERIMENTAL - teach the ym borrow types to speak directly to std
 *          smart pointers. Not included by ymutils.h, not wired into any build
 *          target.
 *
 * This revision replaces the earlier "parallel owning classes" draft. std
 * already owns (std::unique_ptr, std::shared_ptr, whatever comes next), so
 * re-wrapping ownership here would duplicate std's interface and freeze the ym
 * model to today's roster of std types. Instead there are still exactly two
 * ym pointer classes, and the template argument says WHAT is being borrowed:
 *
 *    bound<T>                   -- non-null borrow of a raw T          (ymutils.h, unchanged)
 *    loose<T>                   -- nullable borrow of a raw T          (ymutils.h, unchanged)
 *    bound<std::unique_ptr<T>>  -- non-null borrow of someone else's exclusive owner
 *    bound<std::shared_ptr<T>>  -- non-null borrow of someone else's shared owner
 *    loose<std::unique_ptr<T>>  -- ... and the handle itself may also be absent
 *    loose<std::shared_ptr<T>>
 *    bound<MyHandleType>        -- any ym_Dereferenceable type, std-future-proof
 *
 * An owner-borrow stores a HANDLE (address of the smart-pointer OBJECT that
 * lives inside the existing owner's data) and never copies the target:
 *
 *    - No ownership is taken. Deletion, control blocks, deleters - all remain
 *      std's job. The specializations expose no reset/release/swap/make/deleter
 *      of the owner; there is no std interface to duplicate or maintain.
 *    - Deref PROJECTS through the handle to the pointee at call time
 *      (single deref syntax: *v and v->member read/write the T).
 *    - Re-seating is live: if the owner does owner.reset(newThing), every
 *      existing view immediately sees newThing (the handle address never
 *      changed). If the owner empties itself, views report null and deref
 *      throws ym_NullPtrError - never UB.
 *    - The bound/loose axis covers the HANDLE. The owner's own nullness is a
 *      second dimension that is ALWAYS checked: an owner can be legally reset
 *      after a bound borrow was handed out, so the non-null invariant cannot
 *      extend past the handle. The ym error discipline covers that gap.
 *
 * Conversions keep the raw family's lattice:
 *    bound<Owner> --> loose<Owner>          (implicit, always safe)
 *    .unwrap()    --> bound<T>              (checked projection; throws when nothing is held)
 *    .view()      --> loose<T>              (unchecked projection; null when nothing is held)
 *
 * Lifetime contract: the handle must outlive the view - exactly the contract a
 * raw borrow already carries. Nothing new is introduced.
 *
 * Typical integration (existing codebases keep their std members verbatim):
 * @code
 *    struct Registry {
 *       std::shared_ptr<Device> device;
 *       ym::bound<std::shared_ptr<Device>> getDevice(void) { return device; } // ym_borrow(device) also works
 *    };
 *    void use(void) {
 *       auto dev = registry.getDevice();
 *       dev->open();          // checked through the owner - throws if registry.device was reset()
 *    }
 * @endcode
 *
 * Deviations from the raw family (deliberate):
 *    - No implicit operator T*: a double-deref raw escape hatch is a lifetime
 *      footgun; use .unwrap()/.view() to say which null discipline you want.
 *    - No operator[]: owners own one object; sequences are std::span territory.
 *    - == nullptr / operator bool mean "deref-able NOW" (handle alive AND owner
 *      holds a target), not merely "handle alive".
 *
 * Open questions for the brainstorm:
 *    - Owner-to-owner converting constructors (borrow a unique_ptr as a
 *      shared_ptr view?) - std's aliasing rules are subtle; left out.
 *    - const propagation mirrors std: a const owner still yields mutable T&
 *      (std smart pointers behave the same). Accept or harden?
 *    - Owners of arrays (std::unique_ptr<T[]>) - concept already rejects
 *      weak_ptr; should array owners be a clearer diagnostic?
 *
 * INTEGRATION NOTE (matters): these are partial specializations of
 * ym::BoundPtr / ym::LoosePtr. Every translation unit that instantiates an
 * owner-borrow must see this header; mixing a TU that sees it with one that
 * does not is an ODR violation (primary vs specialization). Promotion path:
 * move the two specializations into ymutils.h and delete this file. The
 * brainstorm lives here so the semantics can be tried first.
 *
 * ENABLING EDIT (already applied to ymutils.h): the wrappers' primaries are now
 * constrained by the named concept ym_Pointable instead of the bare expression
 * (!std::is_array_v<T>) - semantically identical, but constraint subsumption
 * is syntax-based, and both GCC 16 and clang 22 reject any partial
 * specialization against a primary whose constraint is a bare parenthesized
 * expression. Named primary constraint + conjunction spec (the textbook
 * SignedIntegral = Integral && is_signed pattern) is the portable shape.
 */

#pragma once

#include "ymutils.h"

namespace ym
{

// ----------------------------------------------------------------------------
// Owner recognition (duck typed - std-future-proof by construction).
// ----------------------------------------------------------------------------

/**
 * @brief True for the ym borrow types themselves (BoundPtr<U>/LoosePtr<U>).
 *        A borrow is already a view - stacking borrows of borrows adds nothing.
 */
template <typename T>
struct ym_IsYmBorrow : std::false_type { };

template <typename U>
struct ym_IsYmBorrow<BoundPtr<U>> : std::true_type { };

template <typename U>
struct ym_IsYmBorrow<LoosePtr<U>> : std::true_type { };

/**
 * @brief Concept for "someone else's pointer-like object I can borrow a handle
 *        to": dereferenceable with std-style unary star, arrow, and explicit
 *        bool, but not a raw pointer. std::unique_ptr, std::shared_ptr and any
 *        future std addition qualify; std::weak_ptr does not (no unary star),
 *        neither do the ym borrow types themselves nor std::reference_wrapper
 *        (no operator bool to report emptiness with).
 */
template <typename T>
concept ym_Dereferenceable =
   !std::is_pointer_v<T> and
   !std::is_reference_v<T> and
   !std::is_array_v<T> and
   !ym_IsYmBorrow<std::remove_const_t<T>>::value and
   requires (T & owner) {
      { *owner };
      requires std::is_lvalue_reference_v<decltype(*owner)>;
      { owner.operator->() };
      { static_cast<bool>(owner) } -> std::same_as<bool>;
   };

/**
 * @brief What the wrapper specializations treat as a borrowable owner.
 *
 * - The conjunction with ym_Pointable (the primaries' constraint) is what makes
 *   constraint subsumption succeed - do not simplify it away.
 */
template <typename T>
concept ym_BorrowableOwner = ym_Pointable<T> and ym_Dereferenceable<T>;

/**
 * @brief The pointee type reached through an owner
 *        (std::unique_ptr<T, D> / std::shared_ptr<T> -> T).
 */
template <typename Owner_T>
using ym_OwnerTarget = std::remove_reference_t<decltype(*std::declval<Owner_T &>())>;

// ----------------------------------------------------------------------------
// Non-null borrow of an owner.
// ----------------------------------------------------------------------------

/**
 * @brief Specialization: non-null borrow of someone else's pointer-like owner.
 *
 * - The handle (address of the owner object) is guaranteed non-null; the
 *   owner's held target can legally change or go null at any time, so all
 *   access is checked through to the target.
 *
 * @tparam Owner_T -- Any ym_Dereferenceable type (e.g. std::unique_ptr<T, D>).
 */
template <ym_BorrowableOwner Owner_T>
class BoundPtr<Owner_T> : public Ptr_Base<Owner_T>
{
public:
   using Target_T = ym_OwnerTarget<Owner_T>;

   /**
    * @{
    * @brief Constructor.
    *    1. Borrows an existing owner (lvalue - its address is taken).
    *    2. Wraps a raw owner handle. Checked.
    *    3. Wraps a raw owner handle, trusting the caller. Debug-checked only.
    *    4. Rvalue owners are rejected (their address would dangle).
    *
    * @throws ym_NullPtrError -- (2) If handle_Ptr is null. (3) In debug builds only.
    */
   implicit inline BoundPtr(Owner_T & owner) noexcept :
      Ptr_Base<Owner_T>(std::addressof(owner))
   { }

   implicit inline BoundPtr(Owner_T * const handle_Ptr) :
      Ptr_Base<Owner_T>(handle_Ptr)
   {
      YMASSERT(this->get(), ym_NullPtrError, YM_DAH, "Bound owner handle cannot be null");
   }

   implicit inline BoundPtr(
      Owner_T * const handle_Ptr,
      ym_AssumePtrNotNull) YMNOEXC :
         Ptr_Base<Owner_T>(handle_Ptr)
   {
      #if (YM_DEBUG)
         YMASSERT(this->get() != nullptr, ym_NullPtrError, YM_DAH, "Assumed non-null owner handle is null");
      #endif
   }

   BoundPtr(Owner_T &&) = delete; // (4)
   BoundPtr(std::nullptr_t) = delete;
   BoundPtr & operator = (std::nullptr_t) = delete;
   /// @}

   /**
    * @{
    * @brief Checked access projected through the owner to the target.
    *
    * @throws ym_NullPtrError -- If the owner holds null.
    */
   constexpr auto & operator *  (this auto && self) { return *self.require(); }
   constexpr auto * operator -> (this auto && self) { return  self.require(); }
   /// @}

   /**
    * @{
    * @brief Raw-target borrows.
    *
    * @throws ym_NullPtrError -- unwrap() only, if the owner holds null.
    *
    * @returns BoundPtr / LoosePtr of the target type (valid while the target
    *          lives, independent of later owner re-seating).
    */
   BoundPtr<Target_T> unwrap (void) const { return BoundPtr<Target_T>(require()); }
   LoosePtr<Target_T> view   (void) const noexcept { return LoosePtr<Target_T>(try_Ptr()); }
   /// @}

   /// @brief Deref-able now: handle alive (invariant) and owner holds a target.
   constexpr explicit operator bool (void) const noexcept { return try_Ptr() != nullptr; }
   constexpr bool operator == (std::nullptr_t) const noexcept { return try_Ptr() == nullptr; }

private:
   /// @brief Ym discipline: a null target through a borrowed owner is a reported error.
   Target_T * require (this auto && self) {
      YMASSERT(static_cast<bool>(*self._value_ptr), ym_NullPtrError, YM_DAH, "Borrowed owner holds null");
      return std::addressof(*(*self._value_ptr));
   }

   constexpr Target_T * try_Ptr (this auto && self) noexcept {
      return static_cast<bool>(*self._value_ptr) ? std::addressof(*(*self._value_ptr)) : nullptr;
   }
};

// ----------------------------------------------------------------------------
// Nullable borrow of an owner.
// ----------------------------------------------------------------------------

/**
 * @brief Specialization: nullable borrow of someone else's pointer-like owner.
 *
 * - Two null dimensions, both reported (never UB): the handle may be absent,
 *   and the owner may hold null. Access is checked through to the target.
 *
 * @tparam Owner_T -- Any ym_Dereferenceable type (e.g. std::shared_ptr<T>).
 */
template <ym_BorrowableOwner Owner_T>
class LoosePtr<Owner_T> : public Ptr_Base<Owner_T>
{
public:
   using Target_T = ym_OwnerTarget<Owner_T>;

   /**
    * @{
    * @brief Constructor.
    *    1. Empty handle.
    *    2. Borrows an existing owner (lvalue - its address is taken).
    *    3. Wraps a raw owner handle (may be null).
    *    4. Widening from the non-null owner borrow.
    *    5. Rvalue owners are rejected (their address would dangle).
    */
   implicit constexpr LoosePtr(void) noexcept :
      Ptr_Base<Owner_T>(nullptr)
   { }

   implicit constexpr LoosePtr(std::nullptr_t) noexcept :
      Ptr_Base<Owner_T>(nullptr)
   { }

   implicit inline LoosePtr(Owner_T & owner) noexcept :
      Ptr_Base<Owner_T>(std::addressof(owner))
   { }

   implicit constexpr LoosePtr(Owner_T * const handle_Ptr) noexcept :
      Ptr_Base<Owner_T>(handle_Ptr)
   { }

   implicit constexpr LoosePtr(BoundPtr<Owner_T> const & Other) noexcept :
      Ptr_Base<Owner_T>(Other.get())
   { }

   LoosePtr(Owner_T &&) = delete; // (5)
   /// @}

   /**
    * @{
    * @brief Checked access projected through the owner to the target.
    *
    * @throws ym_NullPtrError -- If the handle is null or the owner holds null.
    */
   constexpr auto & operator *  (this auto && self) { return *self.require(); }
   constexpr auto * operator -> (this auto && self) { return  self.require(); }
   /// @}

   /**
    * @{
    * @brief Raw-target borrows.
    *
    * @throws ym_NullPtrError -- unwrap(), if nothing is deref-able.
    */
   BoundPtr<Target_T> unwrap (void) const { return BoundPtr<Target_T>(require()); }

   BoundPtr<Target_T> unwrap_or(BoundPtr<Target_T> const Ptr) const noexcept {
      return (*this) ? unwrap() : Ptr;
   }

   LoosePtr<Target_T> view (void) const noexcept { return LoosePtr<Target_T>(try_Ptr()); }
   /// @}

   /// @brief Replaces the stored handle (std smart pointer parity).
   constexpr void reset(Owner_T * const handle_Ptr = nullptr) noexcept {
      this->_value_ptr = handle_Ptr;
   }

   /// @brief Deref-able now: handle alive AND owner holds a target.
   constexpr explicit operator bool (void) const noexcept { return try_Ptr() != nullptr; }
   constexpr bool operator == (std::nullptr_t) const noexcept { return try_Ptr() == nullptr; }

private:
   /// @brief Ym discipline: either null dimension is a reported error.
   Target_T * require (this auto && self) {
      YMASSERT(self._value_ptr != nullptr, ym_NullPtrError, YM_DAH, "Borrowed owner handle is null");
      YMASSERT(static_cast<bool>(*self._value_ptr), ym_NullPtrError, YM_DAH, "Borrowed owner holds null");
      return std::addressof(*(*self._value_ptr));
   }

   constexpr Target_T * try_Ptr (this auto && self) noexcept {
      return (self._value_ptr && static_cast<bool>(*self._value_ptr))
         ? std::addressof(*(*self._value_ptr))
         : nullptr;
   }
};

// ----------------------------------------------------------------------------
// Borrow helper.
// ----------------------------------------------------------------------------

/**
 * @brief Borrows someone else's std smart pointer without touching ownership.
 *
 * @tparam Owner_T -- Any ym_Dereferenceable type (deduced from the lvalue).
 *
 * @param owner -- The existing owner to borrow (lvalue only).
 *
 * @returns BoundPtr -- Non-null handle borrow (access still checked through).
 */
template <ym_Dereferenceable Owner_T>
constexpr auto ym_borrow(Owner_T & owner) noexcept {
   return BoundPtr<Owner_T>(owner);
}

// ----------------------------------------------------------------------------
// Interface soundness pins.
// ----------------------------------------------------------------------------

/// @brief Owner recognition is duck typed: std's owners qualify, non-owners do not.
static_assert(ym_BorrowableOwner<std::unique_ptr<int>>);
static_assert(ym_BorrowableOwner<std::shared_ptr<int>>);
static_assert(!ym_BorrowableOwner<int>);
static_assert(!ym_BorrowableOwner<bound<int>>);
static_assert(ym_Dereferenceable<std::unique_ptr<int>>);
static_assert(ym_Dereferenceable<std::shared_ptr<int>>);
static_assert(ym_Dereferenceable<std::shared_ptr<int> const>);
static_assert(!ym_Dereferenceable<std::reference_wrapper<int>>); // no operator bool
static_assert(!ym_Dereferenceable<int>);
static_assert(!ym_Dereferenceable<int *>);
static_assert(!ym_Dereferenceable<std::weak_ptr<int>>);
static_assert(!ym_Dereferenceable<int[]>);
static_assert(!ym_Dereferenceable<bound<int>>);
static_assert(!ym_Dereferenceable<loose<char const>>);
static_assert(std::is_same_v<ym_OwnerTarget<std::unique_ptr<int>>, int>);
static_assert(std::is_same_v<ym_OwnerTarget<std::shared_ptr<int const>>, int const>);

/// @brief Null handles and rvalue owners are rejected for bound borrows; the
///        loose owner borrow accepts both.
static_assert(!std::is_constructible_v<bound<std::unique_ptr<int>>, std::nullptr_t>);
static_assert(!std::is_constructible_v<bound<std::unique_ptr<int>>, std::unique_ptr<int>>);
static_assert( std::is_constructible_v<bound<std::unique_ptr<int>>, std::unique_ptr<int> &>);
static_assert( std::is_constructible_v<loose<std::unique_ptr<int>>, std::nullptr_t>);
static_assert(!std::is_constructible_v<loose<std::unique_ptr<int>>, std::unique_ptr<int>>);

/// @brief The handle->owner-borrow widening still exists; raw family untouched.
static_assert(std::is_convertible_v<bound<std::shared_ptr<int>>, loose<std::shared_ptr<int>>>);
static_assert(std::is_convertible_v<std::shared_ptr<int> &, bound<std::shared_ptr<int>>>);
static_assert(std::is_same_v<decltype(std::declval<bound<std::unique_ptr<int>>>().unwrap()), bound<int>>);
static_assert(std::is_same_v<decltype(std::declval<loose<std::shared_ptr<int>>>().view()), loose<int>>);

/// @brief A borrow stays a single pointer (composition via Ptr_Base, no copies).
static_assert(sizeof(bound<std::unique_ptr<int>>) == sizeof(void *));
static_assert(sizeof(loose<std::shared_ptr<int>>) == sizeof(void *));

} // ym
