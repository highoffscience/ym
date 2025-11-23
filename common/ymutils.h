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

/** ymCastPtrTo
 * 
 * @brief Casts given pointer to byte pointer.
 * 
 * @note According to @ref <https://en.cppreference.com/w/cpp/language/object>, any object can be
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
constexpr auto * ymCastPtrTo(U * const data_Ptr)
{
   return static_cast<T *>(
      static_cast<typename std::conditional_t<
         std::is_const_v<U>,
            void const *,
            void *>
         >(data_Ptr));
}

/** ymEmpty
 * 
 * @brief Determines if the parameter is consider empty.
 *
 * @note Can be overloaded for other types.
 * 
 * @returns True if empty, false otherwise.
 */
constexpr auto ymEmpty(rawstr const S)
{
   return static_cast<bool>(!(S && *S));
}

/** ymBinarySearch
 * 
 * @brief Returns an iterator to the searched for element, or last
 *        if no element is found. Range must be in ascending order.
 * 
 * @tparam Iterator_T -- Iterator type.
 * @tparam Compare_T  -- Comparator(Key, It). Key < *It -> < 0;
 *                                            Key = *It -> = 0;
 *                                            Key > *It -> > 0;
 * 
 * @param first   -- Beginning of range.
 * @param last    -- One past the end of the range.
 * @param Value   -- Value to find in range.
 * @param compare -- Compare function. See above.
 */
template <
   typename Iterator_T,
   typename Compare_T = std::less<>>
constexpr auto ymBinarySearch(
   Iterator_T  first,
   Iterator_T  last,
   typename std::iterator_traits<Iterator_T>::value_type const &
               Value,
   Compare_T   compare = Compare_T{})
{
   auto elemIt = last;

   while (first != last)
   { // while there are still elements unchecked
   
      auto const Mid = first + (std::distance(first, last) / 2);
      auto const Cmp = compare(Value, Mid);

      if (Cmp < 0)
      { // Value < *Mid
         last = Mid;
      }
      else if (Cmp > 0)
      { // Value > *Mid
         first = Mid + 1ull;
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
 * @ref <https://en.cppreference.com/w/cpp/types/integer>.
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
   T            * ptr_val{};
   T          * * ptr_ptr_val;
   std::uintptr_t uint_val;
   std::ptrdiff_t diff_val;
};

// ----------------------------------------------------------------------------

/** Bitset
 * 
 * @brief A more compact version of std::bitset.
 * 
 * @note This should only be if std::bitset (which uses u64), is too expensive.
 *       ie, if you only need std::byte, short, or unsigned.
 *
 * @tparam T -- Underlying type.
 */
template <typename T = std::byte>
requires (sizeof(T) < sizeof(std::size_t)) // see above doc comment
class Bitset
{
public:
   /// @brief Constructor.
   explicit constexpr Bitset(void) = default;

   /// @brief True if the bit is set, false otherwise.
   constexpr bool test(T const Idx) const { return _bits & (T(1u) << Idx); }

   /// @brief Sets the bit to 0.
   constexpr void clear(T const Idx) { _bits &= ~(T(1u) << Idx); }

   /// @brief Flips the bit.
   constexpr void flip(T const Idx) { _bits ^= (T(1u) << Idx); }

   /// @brief Flips the bit.
   constexpr void set(T const Idx) { _bits |= (T(1u) << Idx); }

   /// @brief Sets the bit to the specified value.
   constexpr void set(T const Idx, bool const Val) {
      clear(Idx); _bits |= (T(Val) << Idx);
   }

   /// @brief Returns a copy of the underlying data.
   constexpr auto getUnderlying(void) const { return _bits; }

private:
   T _bits{};
};

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
   static_assert(std::is_base_of_v<Ptr_Base, Derived_T>, "Not derived type");

protected:
   /// @brief Wrapper for custom pointer types.
   implicit constexpr Ptr_Base(T * const value_Ptr) :
      _value_ptr {value_Ptr}
   { }

public:
   /// @brief Increments the underlying pointer value.
   constexpr auto operator + (std::integral auto const N) const {
      return Derived_T(_value_ptr + N);
   }

   /// @brief Decrements the underlying pointer value.
   constexpr auto operator - (std::integral auto const N) const {
      return Derived_T(_value_ptr - N);
   }

protected:
   T * _value_ptr{};
};

/// @brief Global null pointer error.
YM_DECL_YMASSERT(NullPtrError)

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
   implicit constexpr BoundPtr_Base(T * const value_Ptr) :
      Ptr_Base<T, Derived_T>(value_Ptr)
   { }

public:
   /// @brief Compile time non-nullness checks.
   constexpr BoundPtr_Base                            (std::nullptr_t) = delete;
   constexpr BoundPtr_Base<T, Derived_T> & operator = (std::nullptr_t) = delete;

   /// @brief Getters.
   constexpr auto * get          (this auto && self) { return  self._value_ptr; }
   constexpr        operator T * (this auto && self) { return  self.get(); }
   constexpr auto & operator *   (this auto && self) { return *self.get(); }
   constexpr auto * operator ->  (this auto && self) { return  self.get(); }

   /// @brief Grabs the element at the specified index. No bounds checking.
   constexpr auto & operator [] (this auto && self, std::integral auto const Idx) {
      return self.get()[Idx];
   }
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
   /// @brief Wrapper for non-null pointer.
   implicit constexpr BoundPtr(T * const value_Ptr) :
      BoundPtr_Base<T, BoundPtr<T>>(value_Ptr)
   {
      YMASSERT(get(), NullPtrError, YM_DAH, "Bound pointer cannot be null");
   }

   /// @brief Enables users to cast pointer to anything.
   YM_MAKE_PASSKEY(CastPassKey)

   /// @brief Casting constructor.
   template <typename U>
   requires (std::is_convertible_v<U*, T*>) // enforce legal casting
   implicit constexpr BoundPtr(BoundPtr<U, Derived_T> const & Other) :
      BoundPtr<T>(Other)
   { }

   /// @brief Casting constructor. Anything goes.
   template <typename U>
   implicit constexpr BoundPtr(
      BoundPtr<U, Derived_T> const & Other,
      CastPassKey            const) :
         BoundPtr<T>(ymCastPtrTo<T>(Other))
   { }

   /// @brief Decaying constructor. Pointer to array to pointer is safe.
   implicit constexpr BoundPtr(BoundPtr<T[]> const Other) :
      BoundPtr_Base<T, BoundPtr<T>>(Other)
   { }

   /// @brief Assignment.
   constexpr auto & operator = (T * const value_Ptr) {
      _value_ptr = BoundPtr(value_Ptr);
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
   implicit constexpr BoundPtr(T (&array) [N]) :
      BoundPtr_Base<T, BoundPtr<T[]>>(array)
   { }

   /// @brief Assignment.
   constexpr auto & operator = (T (&array) [N]) {
      _value_ptr = array;
      return *this;
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
   implicit constexpr FreePtr(void) = default;

   /// @brief Constructor.
   implicit constexpr FreePtr(T * const value_Ptr) :
      Ptr_Base<T, FreePtr<T>>(value_Ptr)
   { }

   /// @brief Assignment.
   constexpr auto & operator = (T * const value_Ptr) {
      _value_ptr = value_Ptr;
      return *this;
   }

   /// @brief Comparison operations.
   constexpr auto operator <=> (FreePtr<T> const &) const noexcept = default;

   /// @brief True if contained pointer is not null, false otherwise.
   constexpr operator bool(void) const noexcept {
      return *this != nullptr;
   }

   /// @brief Returns a BoundPtr to the contained pointer.
   constexpr BoundPtr<T> unwrap(void) {
      return _value_ptr;
   }

   /// @brief Returns a BoundPtr to the contained pointer, or a default value if the contained pointer is null.
   constexpr BoundPtr<T> unwrap_or(BoundPtr<T> const BPtr) {
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
   std::size_t N>
requires (requires(
   Base_T const & Base, BoundPtr<void> const val_BPtr, std::size_t const Size_bytes) {
      { Base.cloneAt(val_BPtr, Size_bytes) };
})
class PolyRaw
{
public:
   explicit constexpr PolyRaw(void) = default;

   constexpr bptr<Base_T> operator -> (void) {
      return bptr(ymCastPtrTo<Base_T>(_buffer.data()));
   }

   constexpr bptr<Base_T const> operator -> (void) const {
      return bptr(ymCastPtrTo<Base_T const>(_buffer.data()));
   }

   /// @brief Copy constructor.
   constexpr PolyRaw(PolyRaw<Base_T, N> const & Other) {
      *this = Other;
   }

   /// @brief Move constructor.
   constexpr PolyRaw(PolyRaw<Base_T, N> && other_uref) {
      *this = other_uref;
   }

   constexpr PolyRaw<Base_T, N> & operator = (PolyRaw<Base_T, N> const & Other) {
      if (this != &Other) { // prevent self assign
         Other->cloneAt(_buffer.data(), N);
      }
      return *this;
   }

   constexpr PolyRaw<Base_T, N> & operator = (PolyRaw<Base_T, N> && other_uref) {
      return *this = other_uref;
   }
   
   template <
      typename    Derived_T,
      typename... Args_T>
   requires (
      std::is_base_of_v<Base_T, Derived_T> &&
      sizeof(Derived_T) <= N)
   constexpr void construct(Args_T &&... args_uref) {
      ::new (_buffer.data()) Derived_T(std::forward<Args_T>(args_uref)...);
   }

private:
   std::array<byte, N> _buffer; // no inline initialization intended
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

} // fmt
