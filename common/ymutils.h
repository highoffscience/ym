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
#include <exception> // TODO if yes exceptions
#include <iterator>
#include <memory>
#include <memory_resource>
#include <type_traits>
#include <utility>

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
 * @tparam T          -- Element type.
 * @tparam Compare_T  -- Comparator(Key, It). Key < *It -> < 0;
 *                                            Key = *It -> = 0;
 *                                            Key > *It -> > 0;
 * 
 * @param first   -- Beginning of range.
 * @param last    -- One past the end of the range.
 * @param Value   -- Value to find in range.
 * @param compare -- Compare function. See above.
 * 
 * @todo T = typename std::iterator_traits<Iterator_T>::value_type should be specialized overload
 */
template <
   typename Iterator_T,
   typename T,
   typename Compare_T>
constexpr auto ymBinarySearch(
   Iterator_T first,
   Iterator_T last,
   T const &  Value,
   Compare_T  compare)
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

/** PtrToInt_T
 *
 * @brief Casts non-member pointer to an appropriately sized uint.
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
requires (!std::is_member_function_pointer<T>::value)
union PtrToInt_T
{
   T       * ptr_val;
   T     * * ptr_ptr_val;
   uintptr * uint_ptr_val;
   uintptr   uint_val;
   intptr  * int_ptr_val;
   intptr    int_val;
};

// ----------------------------------------------------------------------------

/** Bitset
 * 
 * TODO
 */
template <typename T = uint8>
class Bitset
{
public:
   explicit constexpr Bitset(void) = default;

   constexpr bool test  (T const Idx) const { return _bits &   (T(1u) << Idx); }
   constexpr void clear (T const Idx)       {        _bits &= ~(T(1u) << Idx); }
   constexpr void toggle(T const Idx)       {        _bits ^=  (T(1u) << Idx); }
   constexpr void set   (T const Idx)       {        _bits |=  (T(1u) << Idx); }
   constexpr void set   (T const Idx, bool const Val) {
      clear(Idx); _bits |= (T(Val) << Idx);
   }

   constexpr auto   getUnderlying   (void) const { return  _bits; }
   constexpr auto * getUnderlyingPtr(void) const { return &_bits; }

private:
   T _bits{};
};

/** TrustedBoundedPtr
 *
 * @brief Non-null pointer. There is no null check upon construction - pointers
 *        passed to this class are trusted to be non-null.
 * 
 * @tparam T -- Type of pointer.
 */
template <typename T>
class TrustedBoundedPtr
{
public:
   /** TrustedBoundedPtr
    * 
    * @brief Constructs a bounded pointer from an assumed non-null pointer.
    * 
    * @note This class is used to skip the null check in BoundedPtr for cases
    *       where the programmer is guaranteeing non-nullness.
    * 
    * @note Constructor is marked explicit to help the compiler disambiguate
    *       the statement
    *          BoundedPtr s = "s";
    *       The assignment operator can take string literal or TrustedBoundedPtr.
    * 
    * @param t_Ptr -- Pointer to bind.
    */
   explicit constexpr TrustedBoundedPtr(T * const t_Ptr) :
      _t_ptr {t_Ptr}
   { }

   template <typename U>
   requires (std::is_convertible_v<U*, T*>)
   implicit constexpr TrustedBoundedPtr(TrustedBoundedPtr<U> const & Other) :
      TrustedBoundedPtr<T>(static_cast<void*>(Other.get())) // TODO worry about void cont *, too
   { }

   /// @brief Compile time non-nullness checks.
   constexpr TrustedBoundedPtr              (std::nullptr_t) = delete;
   constexpr TrustedBoundedPtr & operator = (std::nullptr_t) = delete;

   constexpr auto * get          (this auto && self) { return  self._t_ptr; }
   constexpr        operator T * (this auto && self) { return  self.get();  }
   constexpr auto & operator *   (this auto && self) { return *self.get();  }
   constexpr auto * operator ->  (this auto && self) { return  self.get();  }

   constexpr auto operator [] (this auto && self, auto const Idx) {
      return self.get()[Idx];
   }

private:
   T * _t_ptr;
};

YM_DECL_YMASSERT(NullPtrError)

/** BoundedPtr
 *
 * @brief Non-null pointer.
 * 
 * @tparam T -- Type of pointer.
 * 
 * @note Throwing in the constructor is preferable because you cannot swallow the
 *       exception and use BoundedPtr in an unacceptable state.
 */
template <typename T>
class BoundedPtr : public TrustedBoundedPtr<T>
{
public:
   /** BoundedPtr
    * 
    * @brief Constructs a bounded pointer from a raw pointer.
    * 
    * @param t_Ptr -- Pointer to bind.
    * 
    * @throws BoundedPtrNullError -- If the parameter is null.
    */
   implicit constexpr BoundedPtr(T * const t_Ptr) :
      TrustedBoundedPtr<T>(t_Ptr)
   {
      YMASSERT(this->get(), NullPtrError, YM_DAH, "Bounded pointer cannot be null");
   }

   // TODO
   // Disallow dropping const when converting to void
      //   !(std::is_void_v<T> &&
      //     std::is_const_v<U> &&
      //     !std::is_const_v<T>)

   template <typename U>
   requires (std::is_convertible_v<U*, T*>)
   implicit constexpr BoundedPtr(BoundedPtr<U> const & Other) :
      BoundedPtr<T>(static_cast<void*>(Other.get())) // TODO worry about void cont *, too
   { }

   /** BoundedPtr
    * 
    * @brief Constructs a bounded pointer from TrustedBoundedPtr.
    * 
    * @note Param tbp is marked non-const because it will take non-const pointers
    *       and incorrectly put them in a const context.
    * 
    * @param tbp -- Pointer to bind.
    * 
    * @throws Whatever BoundedPtr() throws.
    */
   implicit constexpr BoundedPtr(TrustedBoundedPtr<T> tbp) :
      BoundedPtr(tbp.get())
   { }

   /// @brief Compile time non-nullness checks.
   constexpr BoundedPtr              (std::nullptr_t) = delete;
   constexpr BoundedPtr & operator = (std::nullptr_t) = delete;

   /** operator =
    * 
    * @brief Assigns a bounded pointer from TrustedBoundedPtr.
    * 
    * @param TBP -- Pointer to bind.
    * 
    * @returns BoundedPtr -- *this.
    */
   constexpr auto operator = (TrustedBoundedPtr<T> const TBP)
   {
      this->_t_ptr = TBP;
      return *this;
   }
};

/// @brief Convenience alias.
template <typename T>
using tbptr = TrustedBoundedPtr<T>;

/// @brief Convenience alias.
template <typename T>
using bptr = BoundedPtr<T>;

/// @brief Convenience alias.
using str = bptr<char const>;

/// @brief Convenience user-defined literal
constexpr inline auto operator""_str(rawstr const S, std::size_t) { return tbptr(S); }

/**
 * @brief TODO
 */
template <
   typename Base_T,
   sizet N>
requires (requires(
   Base_T const & Base, bptr<void> const val_BPtr, sizet const Size_bytes) {
      { Base.cloneAt(val_BPtr, Size_bytes) };
})
class PolyRaw
{
public:
   explicit constexpr PolyRaw(void) = default;

   constexpr bptr<Base_T> operator -> (void) {
      return tbptr(ymCastPtrTo<Base_T>(_buffer.data()));
   }

   constexpr bptr<Base_T const> operator -> (void) const {
      return tbptr(ymCastPtrTo<Base_T const>(_buffer.data()));
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
