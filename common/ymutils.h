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
#include <functional>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <type_traits>
#include <utility>

#if (YM_YES_EXCEPTIONS)
   #include <exception>
#endif

namespace ym
{

/** ym_castPtrTo
 *
 * @brief Casts given pointer to byte pointer.
 *
 * @note According to @link <https://en.cppreference.com/w/cpp/language/object>, any object can be
 *       inspected assuming an underlying representation of bytes.
 *
 * @note A reinterpret_cast will not convert a pointer of arbitrary type to another, must cast
 *       to void first. We can avoid an explicit cast to void by just accepting a void * since
 *       pointers can be implicitely cast to void.
 *
 * @note U will be either const or non-const.
 *
 * @tparam T -- Data type to cast to.
 * @tparam U -- Deduced data type (implicit).
 *
 * @param data_Ptr -- Pointer to object(s).
 *
 * @returns T (const) * -- Pointer to object(s) represented as an array of T.
 */
template <
   typename T,
   typename U>
constexpr auto * ym_castPtrTo(U * const data_Ptr) noexcept
{
   return static_cast<T *>(
      static_cast<typename std::conditional_t<
         std::is_const_v<U>,
            void const *,
            void *>
         >(data_Ptr));
}

/** ym_empty
 *
 * @brief Determines if the parameter is consider empty.
 *
 * @note Can be overloaded for other types.
 *
 * @returns True if empty, false otherwise.
 */
constexpr auto ym_empty(rawstr const S) noexcept
{
   return static_cast<bool>(!(S && *S));
}

/** ym_binarySearch
 *
 * @brief Returns an iterator to the searched for element, or last
 *        if no element is found. Range must be in ascending order.
 *
 * @tparam Iterator_T -- Iterator type.
 * @tparam Compare_T  -- Comparator(Key, It). Key <=> *It.
 *
 * @param first   -- Beginning of range.
 * @param last    -- One past the end of the range.
 * @param Value   -- Value to find in range.
 * @param compare -- Compare function. See above.
 */
template <
   typename Iterator_T,
   typename Compare_T = std::compare_three_way>
constexpr auto ym_binarySearch(
   Iterator_T  first,
   Iterator_T  last,
   typename std::iterator_traits<Iterator_T>::value_type const &
               Value,
   Compare_T   compare = Compare_T{}) noexcept
requires (
   std::is_nothrow_invocable_v<Compare_T,
      typename std::iterator_traits<Iterator_T>::value_type const &,
      typename std::iterator_traits<Iterator_T>::value_type const &>)
{
   auto elemIt = last;

   while (first != last)
   { // while there are still elements unchecked

      auto const Mid = first + (std::distance(first, last) / 2);
      auto const Cmp = compare(Value, *Mid);

      if (Cmp == std::weak_ordering::less)
      { // Value < *Mid
         last = Mid;
      }
      else if (Cmp == std::weak_ordering::greater)
      { // Value > *Mid
         first = Mid + 1;
      }
      else
      { // Value == *Mid
         elemIt = Mid;
         break;
      }
   }

   return elemIt;
}

// ----------------------------------------------------------------------------

/** PtrInt_T
 *
 * @brief Casts non-member pointer to an appropriately sized integral type.
 *
 * @link <https://en.cppreference.com/w/cpp/types/integer>.
 *
 * @note It is important to make sure the size of the pointer is the exact size of the
 *       type we're trying to cast too. Too small or large will lead to undefined
 *       behaviour and subtle bugs.
 *
 * @note It is unrecommended to store function pointers as a pointer to void, and thus
 *       as a uint. However many compilers allow it because of the days of C, and so
 *       a reinterpret_cast is recommended instead of a static_cast if you really feel
 *       the need to. As long as the type we are casting is the same size we should be
 *       ok, but function pointers are treated differently on some architectures than
 *       data pointers. We explicitly disqualify member function pointers because they
 *       usually occupy 16 bytes. If necessary a convenience casting method similar to
 *       this one can be made and placed in the experimental block but there is no need
 *       and usually cleaner solutions exist.
 *
 * @tparam T -- Pointer type.
 */
template <typename T>
requires (!std::is_member_function_pointer_v<T>)
union PtrInt_T
{
   T            * ptr_val{nullptr};
   T          * * ptr_ptr_val;
   std::uintptr_t uint_val;
   std::ptrdiff_t diff_val;
};

// ----------------------------------------------------------------------------

/** MiniBitset
 *
 * @brief A more compact version of std::bitset.
 *
 * @note This should only be if std::bitset (which uses u64), is too expensive.
 *       ie, if you only need a byte.
 *
 * @tparam T -- Underlying type.
 */
class MiniBitset
{
public:
   /// @brief Constructor.
   explicit constexpr MiniBitset(void) noexcept = default;

   /// @brief True if the bit is set, false otherwise.
   constexpr bool test(std::size_t const Idx) const noexcept {
      return _bits & (static_cast<uchar>(1uz << Idx));
   }

   /// @brief Sets the bit to 0.
   constexpr void clear(std::size_t const Idx) noexcept {
      _bits &= ~(static_cast<uchar>(1uz << Idx));
   }

   /// @brief Flips the bit.
   constexpr void flip(std::size_t const Idx) noexcept {
      _bits ^= (static_cast<uchar>(1uz << Idx));
   }

   /// @brief Flips the bit.
   constexpr void set(std::size_t const Idx) noexcept {
      _bits |= (static_cast<uchar>(1uz << Idx));
   }

   /// @brief Sets the bit to the specified value.
   constexpr void set(std::size_t const Idx, bool const Val) noexcept {
      clear(Idx);
      _bits |= (static_cast<uchar>(static_cast<std::size_t>(Val) << Idx)); // sets/clears bit
   }

   /// @brief Returns a copy of the underlying data.
   constexpr auto getUnderlying(void) const noexcept { return _bits; }

private:
   uchar _bits{0_u8};
};

/// @brief Global null pointer error.
YM_DECL_YMASSERT(ym_NullPtrError)

/// @brief Tag to indicate raw pointer is not null.
YM_CREATE_TAG_DISPATCH_TYPE(ym_AssumePtrNotNull)

/// @brief Enables users to cast pointer to anything.
YM_CREATE_TAG_DISPATCH_TYPE(ym_PtrCastPassKey)

/** Ptr_Base
 *
 * @brief Common operations/fields for pointer wrapper classes.
 *
 * @tparam T         -- Type of pointer.
 * @tparam Derived_T -- Type of derived class.
 */
template <
   typename T,
   typename Derived_T>
class Ptr_Base
{
protected:
   /// @brief Wrapper for custom pointer types.
   implicit constexpr Ptr_Base(T * const value_Ptr) noexcept :
      _value_ptr {value_Ptr}
   { }

public:
   /// @brief Increments the underlying pointer value.
   constexpr auto operator + (std::integral auto const N) const noexcept {
      return Derived_T(_value_ptr + N);
   }

   /// @brief Decrements the underlying pointer value.
   constexpr auto operator - (std::integral auto const N) const noexcept {
      return Derived_T(_value_ptr - N);
   }

protected:
   T * _value_ptr{nullptr};
};

/** BoundPtr
 *
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
   /// @brief Wrapper for non-null pointer.
   implicit constexpr BoundPtr_Base(T * const value_Ptr) noexcept :
      Ptr_Base<T, Derived_T>(value_Ptr)
   { }

public:
   /// @name Creation methods.
   /// @{
   /// @brief Compile time non-nullness checks.
   constexpr BoundPtr_Base                            (std::nullptr_t) = delete;
   constexpr BoundPtr_Base<T, Derived_T> & operator = (std::nullptr_t) = delete;
   /// @}

   /// @name Getters.
   /// @{
   /// @brief Getter.
   constexpr auto * get          (this auto && self) noexcept { return  self._value_ptr; }
   constexpr        operator T * (this auto && self) noexcept { return  self.get(); }
   constexpr auto & operator *   (this auto && self) noexcept { return *self.get(); }
   constexpr auto * operator ->  (this auto && self) noexcept { return  self.get(); }
   /// @}
};

/** BoundPtr
 *
 * @brief Warpper class for non-null pointers. Checked at construction.
 *
 * @note Throwing in the constructor is preferable because you cannot swallow the
 *       exception and use BoundPtr in an unacceptable state.
 */
template <typename T>
class BoundPtr : public BoundPtr_Base<T, BoundPtr<T>>
{
public:
   /// @brief Constructor. Throws if pointer is null.
   implicit constexpr BoundPtr(T * const value_Ptr) :
      BoundPtr_Base<T, BoundPtr<T>>(value_Ptr)
   { // TODO test this constructor for strlit -> str conversions, etc.
      YMASSERT(this->get(), ym_NullPtrError, YM_DAH, "Bound pointer cannot be null");
   }

   /// @brief Constructor. Assumes pointer is not null, like from std::array<>.data().
   implicit constexpr BoundPtr(
      T * const value_Ptr,
      ym_AssumePtrNotNull) noexcept :
         BoundPtr_Base<T, BoundPtr<T>>(value_Ptr)
   { }

   /// @brief Casting constructor.
   template <typename U>
   requires (std::is_convertible_v<U*, T*>) // enforce legal casting
   implicit constexpr BoundPtr(BoundPtr<U> const & Other) noexcept :
      BoundPtr_Base<T, BoundPtr<T>>(ym_castPtrTo<T>(Other.get()))
   { }

   /// @brief Casting constructor. Anything goes.
   template <typename U>
   implicit constexpr BoundPtr(
      BoundPtr<U> const & Other,
      CastPassKey const) noexcept :
         BoundPtr_Base<T, BoundPtr<T>>(ym_castPtrTo<T>(Other.get()))
   { }

   /// @brief Decaying constructor. Pointer to array pointer is safe.
   template <typename U = T>
   requires (!std::is_void_v<std::remove_cv_t<U>>) // void[] is ill-formed
   implicit constexpr BoundPtr(BoundPtr<U[]> const Other) noexcept :
      BoundPtr_Base<T, BoundPtr<T>>(Other)
   { }

   /// @brief Assignment.
   constexpr auto & operator = (T * const value_Ptr) {
      this->_value_ptr = BoundPtr(value_Ptr);
      return *this;
   }
};

/** BoundPtr
 *
 * @brief Wrapper class for pointers to C-style arrays. See note about non-nullness.
 *
 * @note Compiling with the pedantic flag is recommended to prevent allowing arrays
 *       with zero size. If you are using 0-sized arrays, you'll need to modify
 *       the check conditions of this class.
 */
template <typename T>
class BoundPtr<T[]> : public BoundPtr_Base<T, BoundPtr<T[]>>
{
public:
   /// @brief Wrapper for non-null pointer.
   template <std::size_t N>
   implicit constexpr BoundPtr(T (&array) [N]) noexcept :
      BoundPtr_Base<T, BoundPtr<T[]>>(array)
   { }

   /// @brief Constructor from limited lifetime memory to array is unsafe.
   implicit constexpr BoundPtr(BoundPtr<T> const) = delete;

   /// @brief Casting constructor.
   template <typename U>
   requires (std::is_convertible_v<U*, T*>) // enforce legal casting
   implicit constexpr BoundPtr(BoundPtr<U[]> const & Other) noexcept :
      BoundPtr_Base<T, BoundPtr<T[]>>(Other)
   { }

   /// @brief Assignment.
   template <std::size_t N>
   constexpr auto & operator = (T (&array) [N]) noexcept {
      this->_value_ptr = array;
      return *this;
   }

   /// @brief Grabs the element at the specified index. No bounds checking.
   constexpr auto & operator [] (this auto && self, std::integral auto const Idx) noexcept {
      return self.get()[Idx];
   }
};

/// @brief Deduction guide - prevents pointer to array from decaying.
template <typename T, std::size_t N>
BoundPtr(T (&)[N]) -> BoundPtr<T[]>;

/** FreePtr
 *
 * @brief Wrapper class that represents a possibly null pointer. No access is allowed without first
 *        converting to a BoundPtr.
 *
 * @note No need to handle pointer to array cases - if it is an array then a BoundPtr will be made instead.
 */
template <typename T>
class FreePtr : public Ptr_Base<T, FreePtr<T>>
{
public:
   /// @brief Constructor.
   implicit constexpr FreePtr(void) noexcept :
      FreePtr<T>(nullptr)
   { }

   /// @brief Constructor.
   implicit constexpr FreePtr(T * const value_Ptr) noexcept :
      Ptr_Base<T, FreePtr<T>>(value_Ptr)
   { }

   /// @brief Assignment.
   constexpr auto & operator = (T * const value_Ptr) noexcept {
      this->_value_ptr = value_Ptr;
      return *this;
   }

   /// @name Comparison operations.
   /// @{
   /// @brief Comparison overloads.
   constexpr auto operator <=> (FreePtr<T> const &) const noexcept = default;
   constexpr auto operator == (std::nullptr_t) const noexcept { return this->_value_ptr == nullptr; }
   /// @}

   /// @brief True if contained pointer is not null, false otherwise.
   constexpr operator bool(void) const noexcept {
      return *this != nullptr;
   }

   /// @brief Returns a BoundPtr to the contained pointer.
   /// @throws ym_NullPtrError -- If value is null.
   constexpr BoundPtr<T> unwrap(void) {
      return this->_value_ptr;
   }

   /// @brief Returns a BoundPtr to the contained pointer, or a default value if the contained pointer is null.
   constexpr BoundPtr<T> unwrap_or(BoundPtr<T> const BPtr) noexcept {
      return (*this) ? unwrap() : BPtr;
   }
};

/// @brief Convenience alias.
using str = BoundPtr<char const>; // string

/// @brief Convenience alias.
using strlit = BoundPtr<char const[]>; // string literal

/// @brief Convenience alias.
using mutstr = BoundPtr<char>; // mutable string

/** PolyRaw
 *
 * @brief Holds a polymorphic object that share a common base and whose sizes are all equivalent.
 *
 * @tparam Base_T -- Base class.
 * @tparam N      -- Size of derived classes (in bytes).
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
   /// @brief Constructor.
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

   /// @brief Returns const base object pointer.
   constexpr BoundPtr<Base_T const> operator -> (void) const noexcept {
      return {
         std::launder(
            ym_castPtrTo<Base_T const>(_buffer.data())
         ),
         ym_AssumePtrNotNull{} // _buffer guaranteed to have non-zero memory
      };
   }

   /// @brief Returns base object pointer.
   /// TODO why doesn't this implementation match the above one?
   constexpr BoundPtr<Base_T> operator -> (void) noexcept {
      return std::launder(ym_castPtrTo<Base_T>(_buffer.data()));
   }

   /// @brief Copy constructor.
   constexpr PolyRaw(PolyRaw<Base_T, MaxDerivedSize> const & Other) {
      *this = Other;
   }

   /// @brief Copy assignment.
   constexpr PolyRaw<Base_T, MaxDerivedSize> & operator = (PolyRaw<Base_T, MaxDerivedSize> const & Other) {
      if (this != &Other) { // prevent self assign
         Other->cloneAt(_buffer.data(), MaxDerivedSize);
      }
      return *this;
   }

   /// @name Move semantics.
   /// @{
   /// @brief Move constructor & move assignment doesn't make sense for use cases.
   constexpr PolyRaw(PolyRaw<Base_T, MaxDerivedSize> && other) = delete;
   constexpr PolyRaw<Base_T, MaxDerivedSize> & operator = (PolyRaw<Base_T, MaxDerivedSize> && other) = delete;
   /// @}

   /// @brief Constructs derived object in place.
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

/** formatter
 *
 * @brief Helper class to format ym::str types for use in the fmt library.
 */
template <>
struct formatter<ym::str> : public fmt::formatter<fmt::string_view>
{
   auto format(ym::str s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator;
};

/** formatter
 *
 * @brief Helper class to format ym::strlit types for use in the fmt library.
 */
template <>
struct formatter<ym::strlit> : public fmt::formatter<fmt::string_view>
{
   auto format(ym::strlit s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator;
};

/** formatter
 *
 * @brief Helper class to format ym::mutstr types for use in the fmt library.
 */
template <>
struct formatter<ym::mutstr> : public fmt::formatter<fmt::string_view>
{
   auto format(ym::mutstr s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator;
};

} // fmt
