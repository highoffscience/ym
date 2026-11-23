/**
 * @file    ymutils_pointer_brainstorm.cpp
 * @version 0.3.0
 * @author  Forrest Jablonski
 *
 * @brief   EXPERIMENTAL - fully self-contained demo: std::unique_ptr storing a
 *          loose<T> fancy pointer via the Deleter pointer typedef (P1902R2,
 *          C++23). The pointer types below are minimal stand-ins for ym::loose
 *          and ym::bound so the example compiles with nothing but a C++23
 *          standard library.
 *
 * The trick is the `pointer` typedef inside LooseDelete. std::unique_ptr looks
 * for it, finds it, and from then on the stored member IS a loose<Sensor> -
 * one pointer of storage, but the smart pointer's own get()/reset()/release()
 * interfaces now speak the checked type instead of a naked Sensor*.
 *
 * loose<T> models NullablePointer and nothing more: no dereference operators,
 * no raw escape hatch. That deliberately removes the owner's `*owner` and
 * `owner->member` sugar (std's operator* derefs the member, operator-> chains
 * into it) - in exchange there is exactly ONE way to read through this owner,
 * and it is a checked one: get() hands back a loose (a handle that cannot be
 * dereferenced at all), unwrap() turns it into a bound or throws.
 */

#include <cstdio>
#include <exception>
#include <memory>
#include <utility>

namespace
{

/// @brief Thrown when a null pointer tries to become a bound one.
struct NullPtrError : std::runtime_error
{
   NullPtrError(void) : std::runtime_error("bound pointer cannot be null") { }
};

/**
 * @brief Minimal stand-in for ym::bound<T>: a pointer whose constructor
 *        guarantees it is not null.
 */
template <typename T>
class bound
{
public:
   /// @brief Checked conversion from raw - null never becomes a bound.
   /// @throws NullPtrError -- If raw_Ptr is null.
   bound(T * const raw_Ptr) : _raw_ptr {raw_Ptr} {
      if (_raw_ptr == nullptr) { throw NullPtrError{}; }
   }

   /// @brief Dereference accessors - guaranteed safe, no checks needed.
   T & operator * (void) const noexcept { return *_raw_ptr; }
   T * operator -> (void) const noexcept { return _raw_ptr; }

private:
   T * _raw_ptr;
};

/**
 * @brief Minimal stand-in for ym::loose<T>: a possibly-null pointer that models
 *        NullablePointer exactly - default/null constructible, implicit from
 *        raw, comparable, convertible to bool. No dereference operators and no
 *        raw get(): the only way to read is unwrap() into a bound<T>, and the
 *        only way to destroy is delete_resource(), which keeps the deletion
 *        knowledge inside the type that owns the deletion policy.
 */
template <typename T>
class loose
{
public:
   /// @brief Default/null construction is REQUIRED by std - moved-from owners,
   ///        release() and reset() all seat null into the stored member.
   loose(void) = default;
   loose(std::nullptr_t) noexcept { }

   /// @brief Implicit from raw - this is how values enter the smart pointer.
   loose(T * const raw_Ptr) noexcept : _raw_ptr {raw_Ptr} { }

   /// @brief The single read door: null turns into a thrown error.
   /// @throws NullPtrError -- If empty.
   bound<T> unwrap (void) const { return bound<T>(_raw_ptr); }

   /// @brief Deletion is loose's own business - no raw pointer ever escapes.
   ///        The destructor stays trivial on purpose: std's ~unique_ptr runs the
   ///        deleter and THEN assigns pointer(), so a self-deleting destructor
   ///        would double-delete; an explicit method keeps the ordering honest.
   void delete_resource (void) noexcept {
      delete _raw_ptr;
      _raw_ptr = nullptr;
   }

   constexpr explicit operator bool (void) const noexcept { return _raw_ptr != nullptr; }

   /// @brief Comparisons - std's internals ask the member these questions.
   constexpr bool operator == (std::nullptr_t) const noexcept { return _raw_ptr == nullptr; }
   constexpr bool operator == (loose const & Other) const noexcept { return _raw_ptr == Other._raw_ptr; }

private:
   T * _raw_ptr {nullptr};
};

/**
 * @brief The entire integration surface: declare `pointer` and the unique_ptr
 *        member becomes a loose<T>. Deletion is guarded by the type's own bool
 *        and delegated to the type itself.
 */
template <typename T>
struct LooseDelete
{
   using pointer = loose<T>;

   void operator() (pointer p) const {
      if (p) { p.delete_resource(); } // valid pointer only - empty is a no-op
   }
};

struct Sensor
{
   int _id;

   int id(void) const { return _id; }
};

using SensorOwner = std::unique_ptr<Sensor, LooseDelete<Sensor>>;

} // namespace

int main(void)
{
   // 1) make_unique() constructs the value; release() crosses the ownership
   //    boundary, where the raw T* converts implicitly to loose<Sensor>.
   SensorOwner owner(std::make_unique<Sensor>(7));

   // The member is a loose, not a raw pointer...
   static_assert(std::is_same_v<SensorOwner::pointer, loose<Sensor>>);
   static_assert(std::is_same_v<decltype(owner.get()), loose<Sensor>>);

   // ...but it is still exactly one pointer wide.
   static_assert(sizeof(SensorOwner) == sizeof(void *));

   // 2) get() hands back the loose - a handle with NO read surface of its own.
   //    unwrap() is the only door: bound in hand, dereference is check-free.
   loose<Sensor> view = owner.get();
   std::printf("through unwrap():    id=%d\n", view.unwrap()->id());

   // 3) release() gives back the fancy pointer itself, still holding the value.
   loose<Sensor> escaped = owner.release();
   std::printf("through release():   holds value=%d\n", (int)static_cast<bool>(escaped));

   // 4) Re-acquire: the loose moves into a new owner; the loose itself has no
   //    raw escape hatch to mis-wire with.
   owner = SensorOwner(std::move(escaped));
   std::printf("after re-acquire:    id=%d\n", owner.get().unwrap()->id());

   // 5) The payoff over a raw-pointer owner: a moved-from owner is an empty
   //    loose - reportable - not a wild pointer - diagnosable UB.
   SensorOwner thief = std::move(owner);
   std::printf("after move:          owner empty=%d thief id=%d\n",
               (int)(owner.get() == nullptr),
               thief.get().unwrap()->id());

   try
   {
      bound<Sensor> never = owner.get().unwrap(); // empty loose -> throws
      std::printf("UNREACHABLE: built bound from null, id=%d\n", never->id());
      return 1;
   }
   catch (NullPtrError const &)
   {
      std::printf("empty owner:         unwrap threw, as designed\n");
   }

   return 0;
}
