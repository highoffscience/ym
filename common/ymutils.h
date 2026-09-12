/**
 * @file    ymutils.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymassert.h"
#include "ymdefs.h"

#include "fmt/base.h"

#include <array>
#include <cstddef>
#include <cstdlib>
#include <compare>
#include <concepts>
#include <exception>
#include <functional>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <type_traits>
#include <utility>

#if (YM_CPP_STANDARD >= 23) && (YM_LITE == 0)
   #include <stacktrace>
#endif

namespace ym
{

/**
 * @brief Casts given pointer to byte pointer.
 *
 * - According to [Reference](https://en.cppreference.com/w/cpp/language/object), any object can be
 *   inspected assuming an underlying representation of bytes.
 *
 * - A reinterpret_cast will not convert a pointer of arbitrary type to another, must cast
 *   to void first. We can avoid an explicit cast to void by just accepting a void * since
 *   pointers can be implicitely cast to void.
 *
 * - U can be either const or non-const.
 *
 * @tparam To_T   -- Data type to cast to.
 * @tparam From_T -- (Deduced) data type to cast from.
 *
 * @param data_Ptr -- Pointer to object(s).
 *
 * @returns To_T (const) * -- Pointer to object(s) represented as an array of T.
 *
 * __Unit Test__
 * - @ref ym::unit::ymutils::TestSuite::Func_castPtrTo.
 *
 * @test Shall ?
 */
template <
   typename To_T,
   typename From_T>
constexpr auto * ym_castPtrTo(From_T * const data_Ptr) noexcept
{
   return static_cast<To_T *>(
      static_cast<typename std::conditional_t<
         std::is_const_v<From_T>,
            void const *,
            void *>
         >(data_Ptr));
}

/**
 * @brief Returns an iterator to the searched for element, or last
 *        if no element is found. Range must be in ascending order.
 *
 * @note Requires input range to be in lexicographic order.
 *
 * @throws std::exception -- From std::distance(). Compare_T{}() is also allowed to throw.
 *
 * @tparam Iterator_T -- Iterator type.
 * @tparam Compare_T  -- Comparator(Key, It). Key <=> *It.
 *
 * @param first   -- Beginning of range.
 * @param last    -- One past the end of the range.
 * @param Value   -- Value to find in range.
 * @param compare -- Compare function. See above.
 *
 * __Unit Test__
 * - @ref ym::unit::ymutils::TestSuite::Func_binarySearch.
 *
 * @test Shall ?
 */
template <
   typename Iterator_T,
   typename Value_T,
   typename Compare_T = std::compare_three_way>
constexpr auto ym_binarySearch(
   Iterator_T      first,
   Iterator_T      last,
   Value_T const & Value, // key
   Compare_T       compare = Compare_T{}) noexcept
requires (
   std::is_nothrow_invocable_v<Compare_T,
      Value_T /* - - - - - - - - - - - - - - - - - - - - */ const &,
      typename std::iterator_traits<Iterator_T>::value_type const &>)
{
   auto elemIt = last;
   auto mid    = last;

   while (first != last)
   { // while there are still elements unchecked

      mid = first + (std::distance(first, last) / 2);
      auto const Cmp = compare(Value, *mid);

      if (Cmp == std::weak_ordering::less)
      { // Value < *mid
         last = mid;
      }
      else if (Cmp == std::weak_ordering::greater)
      { // Value > *mid
         first = mid + 1;
      }
      else
      { // Value == *mid
         elemIt = mid;
         break;
      }
   }

   return elemIt;
}

// ----------------------------------------------------------------------------

/**
 * @brief Casts non-member pointer to an appropriately sized integral type.
 *
 * - [Reference Guide](https://en.cppreference.com/w/cpp/types/integer).
 *
 * - It is important to make sure the size of the pointer is the exact size of the
 *   type we're trying to cast too. Too small or large will lead to undefined
 *   behaviour and subtle bugs.
 *
 * - It is unrecommended to store function pointers as a pointer to void, and thus
 *   as a uint. However many compilers allow it because of the days of C, and so
 *   a reinterpret_cast is recommended instead of a static_cast if you really feel
 *   the need to. As long as the type we are casting is the same size we should be
 *   ok, but function pointers are treated differently on some architectures than
 *   data pointers. We explicitly disqualify member function pointers because they
 *   usually occupy 16 bytes. If necessary a convenience casting method similar to
 *   this one can be made and placed in the experimental block but there is no need
 *   and usually cleaner solutions exist.
 *
 * @tparam T -- Pointer type.
 *
 * __Unit Test__
 * - @ref ym::unit::ymutils::TestSuite::Class_PtrInt.
 *
 * @test Shall ?
 */
template <typename T>
requires (!std::is_member_function_pointer_v<T>)
union PtrInt_T
{
   T            * _ptr_val{nullptr};
   T          * * _ptr_ptr_val;
   std::uintptr_t _uint_val;
   std::ptrdiff_t _diff_val;
};

// ----------------------------------------------------------------------------

/**
 * @brief A more compact version of std::bitset.
 *
 * - This should only be if std::bitset (which uses u64), is too expensive.
 *   ie, if you only need a byte.
 *
 * @tparam T -- Underlying type.
 *
 * __Unit Test__
 * - @ref ym::unit::ymutils::TestSuite::Class_ByteBitset.
 *
 * @test Shall ?
 */
class ByteBitset
{
public:
   /// @brief Constructor.
   explicit constexpr ByteBitset(void) noexcept = default;

   /**
    * @brief True if the bit is set, false otherwise.
    *
    * @param Idx -- Desired bit position/index.
    *
    * @returns bool -- True if the requested bit is set, false otherwise.
    */
   constexpr bool test(std::size_t const Idx) const noexcept {
      return (std::to_integer<std::size_t>(_bits) & (1uz << Idx)) != 0uz;
   }

   /**
    * @brief Sets the bit to 0.
    *
    * @param Idx -- Desired bit position/index.
    */
   constexpr void clear(std::size_t const Idx) noexcept {
      _bits &= ~static_cast<std::byte>(1uz << Idx);
   }

   /**
    * @brief Flips the bit.
    *
    * @param Idx -- Desired bit position/index.
    */
   constexpr void flip(std::size_t const Idx) noexcept {
      _bits ^= static_cast<std::byte>(1uz << Idx);
   }

   /**
    * @brief Sets the bit.
    *
    * @param Idx -- Desired bit position/index.
    */
   constexpr void set(std::size_t const Idx) noexcept {
      _bits |= static_cast<std::byte>(1uz << Idx);
   }

   /**
    * @brief Sets the bit to the specified value.
    *
    * @param Idx -- Desired bit position/index.
    * @param Val -- Desired value.
    */
   constexpr void set(std::size_t const Idx, bool const Val) noexcept {
      clear(Idx);
      _bits |= (static_cast<std::byte>(Val) << Idx); // sets/clears bit
   }

   /**
    * @brief Returns a copy of the underlying data.
    *
    * @returns auto -- The underlying type used to store the bites.
    */
   constexpr auto getUnderlying(void) const noexcept { return _bits; }

private:
   std::byte _bits{0};
};

/// @brief Global null pointer error.
YM_DECL_YMASSERT(ym_NullPtrError)

/// @brief Tag to indicate raw pointer is not null.
YM_CREATE_TAG_DISPATCH_TYPE(ym_AssumePtrNotNull)

/// @brief Enables users to cast pointer to anything.
YM_CREATE_TAG_DISPATCH_TYPE(ym_PtrCastPassKey)

/**
 * @brief Common operations/fields for pointer wrapper classes.
 *
 * - BoundPtr and LoosePtr are agnostic to ownership.
 *
 * @tparam T         -- Type of pointer.
 * @tparam Derived_T -- Type of derived class.
 */
template <
   typename T,
   typename Derived_T>
class Ptr_Base
{
   template <typename U>
   friend class LoosePtr;

   template <typename U>
   friend class BoundPtr;

protected:
   /**
    * @brief Constructor. Wrapper for custom pointer types.
    *
    * @param value_Ptr -- Pointer value to wrap.
    */
   implicit constexpr Ptr_Base(T * const value_Ptr) noexcept :
      _value_ptr {value_Ptr}
   { }

public:
   /**
    * @name Ptr_Base Arithmetic Functions.
    * @{
    *
    * @brief Returns the value of the incremented/decremented pointer value.
    *
    * @param N -- Value to increment/decrement by.
    *
    * @returns auto -- A new derived pointer value wrapper.
    */
   constexpr auto operator + (std::integral auto const N) const noexcept { return Derived_T(_value_ptr + N); }
   constexpr auto operator - (std::integral auto const N) const noexcept { return Derived_T(_value_ptr - N); }

   constexpr auto & operator += (std::integral auto const N) noexcept { return *this = *this + N; }
   constexpr auto & operator -= (std::integral auto const N) noexcept { return *this = *this - N; }

   constexpr auto & operator ++ (std::integral auto const) noexcept { return *this = *this + 1; }
   constexpr auto & operator -- (std::integral auto const) noexcept { return *this = *this - 1; }
   /// @}

protected:
   T * _value_ptr{nullptr};
};

/**
 * @brief Common operations for bound pointer wrapper classes.
 *
 * @tparam T         -- Type of pointer.
 * @tparam Derived_T -- Type of derived class.
 */
template <
   typename T,
   typename Derived_T>
class BoundPtr_Base : public Ptr_Base<T, Derived_T>
{
protected:
   /**
    * @brief Constructor. Wrapper for custom bound pointer types.
    *
    * @param value_Ptr -- Pointer value to wrap.
    */
   implicit constexpr BoundPtr_Base(T * const value_Ptr) noexcept :
      Ptr_Base<T, Derived_T>(value_Ptr)
   { }

public:
   /// @name BoundPtr_Base Compile Time Nullness Checks.
   /// @{
   constexpr BoundPtr_Base                            (std::nullptr_t) = delete;
   constexpr BoundPtr_Base<T, Derived_T> & operator = (std::nullptr_t) = delete;
   /// @}

   /// @name BoundPtr_Base Getter Functions.
   /// @{
   /// @brief Gets underlying pointer value.
   /// @returns auto (*) -- Underlying pointer value.
   constexpr auto * get          (this auto && self) noexcept { return  self._value_ptr; }
   constexpr        operator T * (this auto && self) noexcept { return  self.get(); }
   constexpr auto & operator *   (this auto && self) noexcept { return *self.get(); }
   constexpr auto * operator ->  (this auto && self) noexcept { return  self.get(); }
   /// @}

   /**
    * @brief Grabs the element at the specified index. No bounds checking.
    *
    * - This, in theory, only belongs to @ref BoundPtr<T[]> classes, since it is important to
    *   disambiguate between pointers to objects vs pointers to array of objects. Some examples
    *   where this function is useful for @ref BoundPtr<T>:
    *    - Many C level functions return T*, but actually represent arrays.
    *    - Conversion between strlit to str, but str is expected to behave like a character array.
    *
    * @param Idx -- Index of desired object.
    *
    * @returns auto & -- Reference to desired object.
    */
   constexpr auto & operator [] (this auto && self, std::integral auto const Idx) noexcept {
      return self.get()[Idx];
   }
};

/**
 * @brief Warpper class for non-null pointers. Checked at construction.
 *
 * - Throwing in the constructor is preferable because you cannot swallow the
 *   exception and use BoundPtr in an unacceptable state.
 *
 * @tparam T -- Type of pointer.
 *
 * __Unit Test__
 * - @ref ym::unit::ymutils::TestSuite::Class_BoundPtr.
 *
 * @test Shall ?
 */
template <typename T>
class BoundPtr : public BoundPtr_Base<T, BoundPtr<T>>
{
public:
   /**
    * @brief Constructor. Wrapper for custom bound pointer types.
    *
    * @throws ym_NullPtrError -- If value_Ptr is null.
    *
    * @param value_Ptr -- Pointer value to wrap.
    */
   implicit constexpr BoundPtr(T * const value_Ptr) :
      BoundPtr_Base<T, BoundPtr<T>>(value_Ptr)
   {
      YMASSERT(this->get(), ym_NullPtrError, YM_DAH, "Bound pointer cannot be null");
   }

   /// @brief Constructor. Assumes pointer is not null, like from std::array<>.data().
   implicit constexpr BoundPtr(
      T * const value_Ptr,
      ym_AssumePtrNotNull) noexcept :
         BoundPtr_Base<T, BoundPtr<T>>(value_Ptr)
   { }

   /// @brief Constructor. Unsafe to convert between the two - deallocation strategies differ.
   template <typename U>
   requires (std::is_array_v<U> && !ByteLikeable<T>)
   implicit constexpr BoundPtr(BoundPtr<U> const & Other) noexcept = delete;

   /// @brief Constructor. Only allow if str - it is de facto usage to treat character arrays as character pointers.
   template <typename U>
   requires (std::is_array_v<U> && ByteLikeable<T>)
   implicit constexpr BoundPtr(BoundPtr<U> const & Other) noexcept :
      BoundPtr<T>(Other.get(), ym_AssumePtrNotNull{})
   { }

   /// @brief Casting constructor.
   template <typename U>
   requires (
      std::is_convertible_v<U*, T*> || // enforce legal casting
      std::is_same_v<T, uchar>      || // casting to byte representation is legal
      std::is_same_v<T, std::byte>)    // ...
   implicit constexpr BoundPtr(BoundPtr<U> const & Other) noexcept :
      BoundPtr_Base<T, BoundPtr<T>>(ym_castPtrTo<T>(Other.get()))
   { }

   /// @brief Casting constructor. Anything goes.
   template <typename U>
   implicit constexpr BoundPtr(
      BoundPtr<U>       const & Other,
      ym_PtrCastPassKey const) noexcept :
         BoundPtr_Base<T, BoundPtr<T>>(ym_castPtrTo<T>(Other.get()))
   { }

   /// @name BoundPtr<> Creation Methods.
   /// @{
   /// @brief Compile time non-nullness checks.
   constexpr BoundPtr                 (std::nullptr_t) = delete;
   constexpr BoundPtr<T> & operator = (std::nullptr_t) = delete;
   /// @}

   /// @name BoundPtr<> Comparison Operations.
   /// @{
   /// @brief Comparison overloads.
   constexpr auto operator <=> (BoundPtr<T> const &) const noexcept = default;
   constexpr bool operator == (std::nullptr_t) const noexcept { return false; }
   /// @}
};

/**
 * @brief Wrapper class for pointers to C-style arrays. See note about non-nullness.
 *
 * @note Compiling with the pedantic flag is recommended to prevent allowing arrays
 *       with zero size. If you are using 0-sized arrays, you'll need to modify
 *       the check conditions of this class.
 *
 * __Unit Test__
 * - @ref ym::unit::ymutils::TestSuite::Class_BoundPtr.
 *
 * @test Shall ?
 */
template <typename T>
class BoundPtr<T[]> : public BoundPtr_Base<T, BoundPtr<T[]>>
{
public:
   /// @brief Constructor. Wrapper for non-null pointer.
   template <std::size_t N>
   implicit constexpr BoundPtr(T (&array) [N]) noexcept :
      BoundPtr_Base<T, BoundPtr<T[]>>(array)
   { }

   /// @brief Casting constructor.
   template <typename U>
   requires (std::is_convertible_v<U*, T*>) // enforce legal casting
   implicit constexpr BoundPtr(BoundPtr<U[]> const & Other) noexcept :
      BoundPtr_Base<T, BoundPtr<T[]>>(Other)
   { }

   /// @name BoundPtr<[]> Creation Methods.
   /// @{
   /// @brief Compile time non-nullness checks.
   constexpr BoundPtr                   (std::nullptr_t) = delete;
   constexpr BoundPtr<T[]> & operator = (std::nullptr_t) = delete;
   /// @}
};

/// @brief Deduction guide - prevents pointer to array from decaying.
template <typename T, std::size_t N>
BoundPtr(T (&)[N]) -> BoundPtr<T[]>;

/**
 * @brief Wrapper class that represents a possibly null pointer. No access is allowed without first
 *        converting to a @ref BoundPtr.
 *
 * __Unit Test__
 * - @ref ym::unit::ymutils::TestSuite::Class_LoosePtr.
 *
 * @test Shall ?
 */
template <typename T>
class LoosePtr : public Ptr_Base<T, LoosePtr<T>>
{
public:
   /// @brief Constructor.
   implicit constexpr LoosePtr(void) noexcept :
      LoosePtr<T>(nullptr)
   { }

   /// @brief Constructor.
   implicit constexpr LoosePtr(T * const value_Ptr) noexcept :
      Ptr_Base<T, LoosePtr<T>>(value_Ptr)
   { }

   /// @brief Constructor.
   implicit constexpr LoosePtr(BoundPtr<T> const value_Ptr) noexcept :
      Ptr_Base<T, LoosePtr<T>>(value_Ptr.get())
   { }

   /// @brief Casting constructor.
   template <typename U>
   requires (
      std::is_convertible_v<U*, T*> || // enforce legal casting
      std::is_same_v<T, uchar>      || // casting to byte representation is legal
      std::is_same_v<T, std::byte>)    // ...
   implicit constexpr LoosePtr(LoosePtr<U> const & Other) noexcept :
      Ptr_Base<T, LoosePtr<T>>(ym_castPtrTo<T>(Other._value_ptr))
   { }

   /// @brief Casting constructor. Anything goes.
   template <typename U>
   implicit constexpr LoosePtr(
      LoosePtr<U>       const & Other,
      ym_PtrCastPassKey const) noexcept :
         Ptr_Base<T, LoosePtr<T>>(ym_castPtrTo<T>(Other._value_ptr))
   { }

   /// @brief Constructor. Unsafe to convert between the two - deallocation strategies differ.
   template <typename U>
   requires (std::is_array_v<U> && !ByteLikeable<T>)
   implicit constexpr LoosePtr(LoosePtr<U> const & Other) noexcept = delete;

   /// @brief Constructor. Only allow if str - it is de facto usage to treat character arrays as character pointers.
   template <typename U>
   requires (std::is_array_v<U> && ByteLikeable<T>)
   implicit constexpr LoosePtr(LoosePtr<U> const & Other) noexcept :
      LoosePtr<T>(Other.get())
   { }

   /// @brief Constructor. Unsafe to convert between the two - deallocation strategies differ.
   template <typename U>
   requires (std::is_array_v<U> && !ByteLikeable<T>)
   implicit constexpr LoosePtr(BoundPtr<U> const & Other) noexcept = delete;

   /// @brief Constructor. Only allow if str - it is de facto usage to treat character arrays as character pointers.
   template <typename U>
   requires (std::is_array_v<U> && ByteLikeable<T>)
   implicit constexpr LoosePtr(BoundPtr<U> const & Other) noexcept :
      LoosePtr<T>(Other.get())
   { }

   /// @name LoosePtr Comparison Operations.
   /// @{
   /// @brief Comparison overloads.
   constexpr auto operator <=> (LoosePtr<T> const &) const noexcept = default;
   constexpr bool operator == (std::nullptr_t) const noexcept { return this->_value_ptr == nullptr; }
   /// @}

   /// @brief Returns true if contained pointer is not null, false otherwise.
   constexpr operator bool(void) const noexcept {
      return *this != nullptr;
   }

   /**
    * @brief Returns a @ref BoundPtr to the contained pointer.
    *
    * @throws ym_NullPtrError -- If value is null.
    *
    * @returns BoundPtr<T> -- A BoundPtr of the underlying pointer value.
    */
   constexpr BoundPtr<T> unwrap(void) const {
      return this->_value_ptr;
   }

   /**
    * @brief Returns a @ref BoundPtr to the contained pointer, or a default value if the contained pointer is null.
    *
    * @returns BoundPtr<T> -- A BoundPtr of the underlying pointer value.
    */
   constexpr BoundPtr<T> unwrap_or(BoundPtr<T> const Ptr) const noexcept {
      return (*this) ? unwrap() : Ptr;
   }
};

/**
 * @brief Wrapper class that represents a possibly null pointer. No access is allowed without first
 *        converting to a BoundPtr.
 *
 * __Unit Test__
 * - @ref ym::unit::ymutils::TestSuite::Class_LoosePtr.
 *
 * @test Shall ?
 */
template <typename T>
class LoosePtr<T[]> : public Ptr_Base<T, LoosePtr<T[]>>
{
public:
   /// @brief Constructor.
   implicit constexpr LoosePtr(void) noexcept :
      Ptr_Base<T, LoosePtr<T[]>>(nullptr)
   { }

   /// @brief Wrapper for non-null pointer.
   template <std::size_t N>
   implicit constexpr LoosePtr(T (&array) [N]) noexcept :
      Ptr_Base<T, LoosePtr<T[]>>(array)
   { }

   /// @brief Constructor.
   implicit constexpr LoosePtr(BoundPtr<T[]> const value_Ptr) noexcept :
      Ptr_Base<T, LoosePtr<T[]>>(value_Ptr.get())
   { }

   /// @brief Casting constructor.
   template <typename U>
   requires (std::is_convertible_v<U*, T*>) // enforce legal casting
   implicit constexpr LoosePtr(LoosePtr<U[]> const & Other) noexcept :
      Ptr_Base<T, LoosePtr<T[]>>(Other)
   { }

   /**
    *  @brief Grabs the element at the specified index. No bounds checking.
    *
    * @param Idx -- Index of desired object.
    *
    * @returns auto & -- Reference to desired object.
    */
   constexpr auto & operator [] (this auto && self, std::integral auto const Idx) noexcept {
      return self.get()[Idx];
   }
};

/// @brief Deduction guide - prevents pointer to array from decaying.
template <typename T, std::size_t N>
LoosePtr(T (&)[N]) -> LoosePtr<T[]>;

/// @name Pointer aliases.
/// @{
/// @brief Convenience alias.
template <typename T>
using bound = BoundPtr<T>;

template <typename T>
using loose = LoosePtr<T>;
/// @}

/// @name C-style string aliases.
/// @{
/// @brief Convenience alias.
using str       = BoundPtr<char const>;   // string
using strlit    = BoundPtr<char const[]>; // string literal
using mutstr    = BoundPtr<char>;         // mutable string
using optstr    = LoosePtr<char const>;    // optional string
using optstrlit = LoosePtr<char const[]>;  // optional string literal
using optmutstr = LoosePtr<char>;          // optional mutable string
/// @}

/**
 * @brief Holds a polymorphic object that share a common base and whose sizes are all equivalent.
 *
 * @tparam Base_T -- Base class.
 * @tparam N      -- Size of derived classes (in bytes).
 *
 * __Unit Test__
 * - @ref ym::unit::ymutils::TestSuite::Class_PolyRaw.
 *
 * @test Shall ?
 */
template <
   typename    Base_T,
   std::size_t MaxDerivedSize>
requires (requires(
   Base_T         const & Base,
   BoundPtr<void> const   val,
   std::size_t    const   Size_bytes) {
      { Base.cloneAt(val, Size_bytes) };
      MaxDerivedSize >= sizeof(Base_T);
   }
)
class PolyRaw
{
public:
   /**
    * @brief Constructor.
    *
    * @throws std::exception -- From construct().
    *
    * @param args -- Arguments to forward in construction of derived type.
    */
   template <
      typename    Derived_T,
      typename... Args_T>
   requires (
      std::is_base_of_v<Base_T, Derived_T> &&
      sizeof(Derived_T) <= MaxDerivedSize)
   explicit constexpr PolyRaw(
      std::in_place_type_t<Derived_T>,
      Args_T &&... args)
   {
      construct(std::in_place_type<Derived_T>, std::forward<Args_T>(args)...);
   }

   /**
    * @name PolyRaw Access Operations.
    * @{
    *
    * @brief Returns (const) base object pointer.
    *
    * @return BoundPtr<> -- Bound pointer to underlying object.
    */
   constexpr BoundPtr<Base_T const> operator -> (void) const noexcept {
      return {
         std::launder(
            ym_castPtrTo<Base_T const>(_buffer.data())
         ),
         ym_AssumePtrNotNull{} // _buffer guaranteed to have non-zero memory
      };
   }

   constexpr BoundPtr<Base_T> operator -> (void) noexcept {
      return std::launder(ym_castPtrTo<Base_T>(_buffer.data()));
   }

   /// @}

   /// @brief Copy constructor.
   constexpr PolyRaw(PolyRaw<Base_T, MaxDerivedSize> const & Other) {
      *this = Other;
   }

   /// @brief Copy assignment.
   constexpr auto & operator = (PolyRaw<Base_T, MaxDerivedSize> const & Other) {
      if (this != &Other) { // prevent self assign
         Other->cloneAt(_buffer.data(), MaxDerivedSize);
      }
      return *this;
   }

   /// @name PolyRaw Move Semantics.
   /// @{
   /// @brief Move constructor & move assignment doesn't make sense for use cases.
   constexpr PolyRaw(PolyRaw<Base_T, MaxDerivedSize> && other) = delete;
   constexpr PolyRaw<Base_T, MaxDerivedSize> & operator = (PolyRaw<Base_T, MaxDerivedSize> && other) = delete;
   /// @}

   /**
    * @brief Constructs derived object in place.
    *
    * @throws std::exception -- From Derived_T's constructor.
    *
    * @param args -- Arguments to forward in construction of derived type.
    */
   template <
      typename    Derived_T,
      typename... Args_T>
   requires (
      std::is_base_of_v<Base_T, Derived_T> &&
      sizeof(Derived_T) <= MaxDerivedSize)
   constexpr void construct(
      std::in_place_type_t<Derived_T>,
      Args_T &&... args) {
         ::new (_buffer.data()) Derived_T(std::forward<Args_T>(args)...);
   }

private:
   alignas(std::max_align_t) std::array<std::byte, MaxDerivedSize> _buffer{};
};

} // ym

// ----------------------------------------------------------------------------

namespace fmt
{

/**
 * @brief Helper class to format ym::str types for use in the fmt library.
 */
template <>
struct formatter<ym::str> : public fmt::formatter<fmt::string_view>
{
   auto format(ym::str s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator;
};

/**
 * @brief Helper class to format ym::strlit types for use in the fmt library.
 */
template <>
struct formatter<ym::strlit> : public fmt::formatter<fmt::string_view>
{
   auto format(ym::strlit s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator;
};

/**
 * @brief Helper class to format ym::mutstr types for use in the fmt library.
 */
template <>
struct formatter<ym::mutstr> : public fmt::formatter<fmt::string_view>
{
   auto format(ym::mutstr s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator;
};

#if (YM_CPP_STANDARD >= 23) && (YM_LITE == 0)

   /**
    * @brief Helper class to format ym::mutstr types for use in the fmt library.
    */
   template <>
   struct formatter<std::stacktrace> : public fmt::formatter<fmt::string_view>
   {
      auto format(std::stacktrace s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator;
   };

#endif

} // fmt
