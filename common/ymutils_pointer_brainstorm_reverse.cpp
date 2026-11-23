/**
 * @file    ymutils_pointer_brainstorm_reverse.cpp
 * @version 0.1.0
 * @author  Forrest Jablonski
 *
 * @brief   EXPERIMENTAL - the reverse investigation: instead of teaching std
 *          smart pointers to store ym pointers (ymutils_pointer_brainstorm.h),
 *          the ym pointers OWN a std smart pointer as their member:
 *
 *             bound<std::unique_ptr<T>>   -- exclusively owning, move-only, non-null
 *             bound<std::shared_ptr<T>>   -- shared owning, copyable,   non-null
 *             loose<std::unique_ptr<T>>   -- ...and the same, but nullable
 *             loose<std::shared_ptr<T>>   -- nullable shared owner
 *
 *          The member IS a real std smart pointer (that is "our own fancy
 *          pointer under the hood": one owner-wide of storage, dereference
 *          projected through the member). The wrapper holds it BY VALUE, so the
 *          smart pointer's ownership semantics are inherited for free - every
 *          special member is `= default` and forwards to the member. Defaulted
 *          but impossible copies are implicitly deleted: bound<unique_ptr<T>>
 *          is move-only automatically, bound<shared_ptr<T>> copies-and-shares
 *          automatically. No concepts, no trait plumbing, nothing to maintain
 *          when a new std owner shows up.
 *
 *          Null discipline mirrors the ym family, with one honest amendment:
 *          std's move semantics legally empty the internal owner (a moved-from
 *          unique_ptr is null), and no invariant can survive that. So instead
 *          of pretending, access is CHECKED THROUGH: bound guarantees non-null
 *          at construction, but every read still asks the member - a read
 *          through a moved-from or released owner is a thrown error, never UB.
 *          loose adds the second null dimension (maybe-no-owner) and the usual
 *          single read door: unwrap() into bound<T*>.
 */

#include <cstdio>
#include <exception>
#include <memory>
#include <type_traits>
#include <utility>

namespace
{

/// @brief Thrown when a null owner tries to become a bound one, or is read.
struct NullPtrError : std::runtime_error
{
   NullPtrError(void) : std::runtime_error("owner holds null") { }
};

/**
 * @brief Non-null owner of an owner. Ownership semantics flow in from the
 *        wrapped smart pointer via the defaulted special members.
 */
template <typename Owner_T> // std::unique_ptr<T>, std::shared_ptr<T>, or raw T*
class bound
{
public:
   using element_type = typename std::pointer_traits<Owner_T>::element_type;

   /// @brief Moves the owner in and guarantees it holds a target.
   /// @throws NullPtrError -- If the incoming owner is empty.
   explicit bound(Owner_T owner) : _owner {std::move(owner)} {
      if (!_owner) { throw NullPtrError{}; }
   }

   /// @brief Ownership forwards to the member - move-only owners keep this
   ///        wrapper move-only, copyable owners make it copyable. No concepts:
   ///        a defaulted copy of a move-only member is simply deleted.
   bound (bound &&) = default;
   bound (bound const &) = default;
   bound & operator = (bound &&) = default;
   bound & operator = (bound const &) = default;
   ~bound (void) = default;

   /// @{
   /// @brief Checked-through dereference (see header: moves can legally empty
   ///        the member, so reads verify it).
   /// @throws NullPtrError -- If the member was emptied by a move/release.
   element_type & operator * (void) const { require(); return *_owner; }
   element_type * operator -> (void) const { return require(); }
   /// @}

   /// @brief The usual single read door to the raw target: checked, non-owning.
   /// @throws NullPtrError -- If the member was emptied by a move/release.
   bound<element_type *> unwrap (void) const { return bound<element_type *>(require()); }

   /// @brief Hands the owner itself back to the caller. Leaves this wrapper
   ///        reading as empty (every later read throws) - release is terminal.
   Owner_T release (void) { return std::move(_owner); }

   explicit operator bool (void) const noexcept { return static_cast<bool>(_owner); }

private:
   /// @brief Ask the member, never assume. to_address() unifies raw and smart.
   element_type * require (void) const {
      if (!_owner) { throw NullPtrError{}; }
      return std::to_address(_owner);
   }

   Owner_T _owner;
};

/**
 * @brief Nullable owner of an owner. Same ownership inheritance, plus the
 *        maybe-empty handle state.
 */
template <typename Owner_T>
class loose
{
public:
   using element_type = typename std::pointer_traits<Owner_T>::element_type;

   /// @brief Empty owner, or move one in without requiring a target.
   loose (void) = default;
   explicit loose(Owner_T owner) : _owner {std::move(owner)} { }

   loose (loose &&) = default;
   loose (loose const &) = default;
   loose & operator = (loose &&) = default;
   loose & operator = (loose const &) = default;
   ~loose (void) = default;

   /// @{
   /// @brief Checked dereference - std smart pointer ergonomics, ym error
   ///        discipline: empty is a reported error, not UB.
   /// @throws NullPtrError -- If empty (no owner, or owner holds null).
   element_type & operator * (void) const { require(); return *_owner; }
   element_type * operator -> (void) const { return require(); }
   /// @}

   /// @brief The single unchecked-free read door.
   /// @throws NullPtrError -- If empty.
   bound<element_type *> unwrap (void) const { return bound<element_type *>(require()); }

   /// @brief Hands the owner itself back to the caller (terminal, as in bound).
   Owner_T release (void) { return std::move(_owner); }

   explicit operator bool (void) const noexcept { return static_cast<bool>(_owner); }

private:
   element_type * require (void) const {
      if (!_owner) { throw NullPtrError{}; }
      return std::to_address(_owner);
   }

   Owner_T _owner {};
};

struct Sensor
{
   int _id;

   int id(void) const { return _id; }
};

} // namespace

int main(void)
{
   // 1) Exclusive: wrapping unique_ptr makes the wrapper exclusive and move-only.
   using ExclusiveBound = bound<std::unique_ptr<Sensor>>;
   ExclusiveBound exclusive {std::make_unique<Sensor>(7)};

   static_assert(!std::is_copy_constructible_v<ExclusiveBound>);      // inherited: move-only
   static_assert( std::is_move_constructible_v<ExclusiveBound>);
   static_assert(sizeof(ExclusiveBound) == sizeof(std::unique_ptr<Sensor>)); // one pointer

   std::printf("bound<unique_ptr>:   id=%d\n", exclusive->id());

   try
   {
      ExclusiveBound refused {std::unique_ptr<Sensor>{}}; // empty owner in -> refused
      (void)refused;
      std::printf("UNREACHABLE: empty owner became bound\n");
      return 1;
   }
   catch (NullPtrError const &)
   {
      std::printf("null construction:  refused, as designed\n");
   }

   // 2) Shared: wrapping shared_ptr makes the wrapper copyable and sharing.
   using SharedBound = bound<std::shared_ptr<Sensor>>;
   SharedBound shared {std::make_shared<Sensor>(8)};
   SharedBound twin = shared; // copies the OWNER, not the Sensor - one target, two owners

   static_assert(std::is_copy_constructible_v<SharedBound>);          // inherited: shared
   std::printf("bound<shared_ptr>:   twin sees id=%d\n", twin->id());

   // release() retires the wrapper - note the sequencing: read BEFORE releasing
   // (in one expression, argument evaluation order would be unspecified).
   unsigned long const owners = twin.release().use_count();           // released co-owner still counts: 2
   std::printf("after release():     shared still id=%d, owners=%lu\n", shared->id(), owners);

   // 3) Nullable shared owner: same semantics, minus the non-null invariant.
   loose<std::shared_ptr<Sensor>> maybe {std::make_shared<Sensor>(9)};
   std::printf("loose<shared_ptr>:   holds=%d via-unwrap id=%d\n",
               (int)static_cast<bool>(maybe),
               maybe.unwrap()->id());

   loose<std::unique_ptr<Sensor>> nothing; // move-only handle, empty
   static_assert(!std::is_copy_constructible_v<decltype(nothing)>);
   try
   {
      auto view = nothing.unwrap(); // empty -> throws
      (void)view;
      std::printf("UNREACHABLE: unwrapped empty loose\n");
      return 1;
   }
   catch (NullPtrError const &)
   {
      std::printf("empty loose:        unwrap threw, as designed\n");
   }

   // 4) The checked-through amendment: a moved-from bound is an empty member,
   //    and reads REPORT it instead of invoking UB.
   ExclusiveBound heir = std::move(exclusive);
   std::printf("after move:          heir id=%d, old holder reads=", heir->id());
   try
   {
      (void)exclusive->id();
      std::printf("NO (UNREACHABLE)\n");
      return 1;
   }
   catch (NullPtrError const &)
   {
      std::printf("THROW\n");
   }

   return 0;
}
