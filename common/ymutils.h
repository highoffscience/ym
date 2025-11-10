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
requires (!std::is_member_function_pointer<T>::value) // TODO no _v overload option?
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
 * @brief A more compact version of std::bitset.
 * 
 * @note This should only be if std::bitset (which uses u64), is too expensive.
 *       ie, if you only need std::byte, short, or unsigned.
 *
 * @tparam T -- Underlying type.
 */
template <typename T = byte>
requires (sizeof(T) < sizeof(sizet)) // see above doc comment
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

   constexpr auto const & getUnderlying   (void) const { return  _bits; }
   constexpr auto       * getUnderlyingPtr(void) const { return &_bits; }

private:
   T _bits{};
};

YM_DECL_YMASSERT(NullPtrError)

template <
   typename T>
class BoundPtr_Base
{
public:
   /** BoundPtr
    * 
    * @brief Wrapper for non-null pointer.
    * 
    * @param t_Ptr -- Pointer to bind.
    */
   constexpr BoundPtr_Base(T * const t_Ptr) :
      _t_ptr {t_Ptr}
   {
      if (!get())
      {
         std::terminate();
      }
   }

   /// @brief Casting constructor.
   // template <typename U>
   // constexpr BoundPtr(BoundPtr<U, N> const & Other) :
   //    BoundPtr<T, N>(Other)
   // { }

   /// @brief Compile time non-nullness checks.
   constexpr BoundPtr_Base              (std::nullptr_t) = delete;
   constexpr BoundPtr_Base & operator = (std::nullptr_t) = delete;

   constexpr auto * get          (this auto && self) { return  self._t_ptr; }
   constexpr        operator T * (this auto && self) { return  self.get();  } // TODO can this return T const *?
   constexpr auto & operator *   (this auto && self) { return *self.get();  }
   constexpr auto * operator ->  (this auto && self) { return  self.get();  }

   // constexpr auto decay(void) const {
   //    return BoundPtr<T>(get());
   // }

   // /// @brief Decay array pointer - safe.
   // constexpr operator BoundPtr<T> (void) const {
   //    return decay();
   // }

   constexpr auto & operator [] (this auto && self, std::integral auto const Idx) {
      return self.get()[Idx];
   }

   // friend constexpr auto operator + (BoundPtr<T> const & Lhs, std::integral auto const Rhs) {
   //    return BoundPtr(Lhs.get() + Rhs);
   // }

   // friend constexpr auto operator - (BoundPtr<T> const & Lhs, std::integral auto const Rhs) {
   //    return BoundPtr(Lhs.get() - Rhs);
   // }

private:
   T * _t_ptr;
};

template <
   typename T>
class BoundPtr : public BoundPtr_Base<T>
{
public:
   /** BoundPtr
    * 
    * @brief Wrapper for non-null pointer.
    * 
    * @param t_Ptr -- Pointer to bind.
    */
   constexpr BoundPtr(T * const t_Ptr) :
      BoundPtr_Base<T>(t_Ptr)
   { }

   constexpr BoundPtr(BoundPtr<T[]> const Other) :
      BoundPtr_Base<T>(Other)
   { }
};

template <typename T>
class BoundPtr<T[]> : public BoundPtr_Base<T>
{
public:
   /** BoundPtr
    * 
    * @brief Wrapper for non-null pointer.
    * 
    * @param t_Ptr -- Pointer to bind.
    */
   template <std::size_t N>
   constexpr BoundPtr(T (&t_Ptr) [N]) :
      BoundPtr_Base<T>(t_Ptr)
   { }
};

/// @brief Deduction guide - prevents pointer to array from decaying.
template <typename T, std::size_t N>
BoundPtr(T (&)[N]) -> BoundPtr<T[]>;

/** BoundPtr
 *
 * @brief Non-null pointer. There is no null check upon construction - pointers
 *        passed to this class are trusted to be non-null.
 * 
 * @tparam T -- Type of pointer.
 * @tparam N -- Size of array if pointer to C-style array, 0 otherwise.
 *
 * @note Compiling with the pedantic flag is recommended to prevent allowing arrays
 *       with zero size. If you are using 0-sized arrays, you'll need to modify
 *       the check conditions of this class.
 *
 * @note Throwing in the constructor is preferable because you cannot swallow the
 *       exception and use BoundPtr in an unacceptable state.
 */
// template <
//    typename T,
//    sizet    N = 0uz>
// class BoundPtr
// {
// public:
//    /// @brief Enables users to cast pointer to anything.
//    YM_MAKE_PASSKEY(CastPassKey)

//    /** BoundPtr
//     * 
//     * @brief Wrapper for non-null pointer.
//     * 
//     * @param t_Ptr -- Pointer to bind.
//     */
//    implicit constexpr BoundPtr(T * const t_Ptr) :
//       _t_ptr {t_Ptr}
//    {
//       if constexpr (N == 0uz)
//       { // non-array pointer
//          YMASSERT(this->get(), NullPtrError, YM_DAH, "Bound pointer cannot be null");
//       }
//       else
//       { // pointer to array
//          // arrays are non-null in C++ - see above doc comment for handling 0-sized arrays.
//       }
//    }

//    /// @brief Casting constructor.
//    template <typename U>
//    requires (
//       // TODO does is_convertible handle constness? I don't think so
//       std::is_convertible_v<U*, T*> && // enforce legal casting
//       (std::is_const_v<T>           || // can always convert to const
//       !std::is_const_v<U>))            // else neither should be const
//    implicit constexpr BoundPtr(BoundPtr<U, N> const & Other) :
//       BoundPtr<T, N>(Other)
//    { }

//    /// @brief Casting constructor. Anything goes.
//    template <typename U>
//    implicit constexpr BoundPtr(
//       BoundPtr<U, N> const & Other,
//       CastPassKey    const &) :
//          BoundPtr<T, N>(ymCastPtrTo<T>(Other))
//    { }

//    /// @brief Compile time non-nullness checks.
//    constexpr BoundPtr              (std::nullptr_t) = delete;
//    constexpr BoundPtr & operator = (std::nullptr_t) = delete;

//    constexpr auto * get          (this auto && self) { return  self._t_ptr; }
//    constexpr        operator T * (this auto && self) { return  self.get();  } // TODO can this return T const *?
//    constexpr auto & operator *   (this auto && self) { return *self.get();  }
//    constexpr auto * operator ->  (this auto && self) { return  self.get();  }

//    constexpr auto decay(void) const {
//       return BoundPtr<T>(get());
//    }

//    /// @brief Decay array pointer - safe.
//    constexpr operator BoundPtr<T> (void) const {
//       return decay();
//    }

//    constexpr auto & operator [] (this auto && self, std::integral auto const Idx) {
//       return self.get()[Idx];
//    }

//    friend constexpr auto operator + (BoundPtr<T, N> const & Lhs, std::integral auto const Rhs) {
//       return BoundPtr(Lhs.get() + Rhs);
//    }

//    friend constexpr auto operator - (BoundPtr<T, N> const & Lhs, std::integral auto const Rhs) {
//       return BoundPtr(Lhs.get() - Rhs);
//    }

// private:
//    T * _t_ptr;
// };

// /// @brief Deduction guide - prevents pointer to array from decaying.
// template <typename T, sizet N>
// BoundPtr(T (&)[N]) -> BoundPtr<T, N>;

/**
 * TODO
 *
 * @note No need to handle pointer to array cases - if it is an array then a BoundPtr will be made instead.
 */
template <typename T>
class FreePtr
{
public:
   constexpr FreePtr(T * const t_Ptr) :
      _t_ptr {t_Ptr}
   { }

   constexpr bool hasValue(void) const {
      return _t_ptr != nullptr;
   }

   constexpr BoundPtr<T> unwrap(void) {
      return _t_ptr;
   }

   constexpr BoundPtr<T> unwrap_or(BoundPtr<T> const BPtr) {
      return hasValue() ? unwrap() : BPtr;
   }

private:
   T * _t_ptr{};
};

/// @brief Convenience alias.
template <typename T>
using bptr = BoundPtr<T>;

/// @brief Convenience alias.
template <typename T>
using fptr = FreePtr<T>;

/// @brief Convenience alias.
using str = BoundPtr<char const>; // string

// TODO maybe provide an overload to boundedptr that takes char (&Format)[N]
//      and static_assert N > 0
/// @brief Convenience user-defined literal
// constexpr inline auto operator""_str(rawstr const S, std::size_t) { return str(S); }

/** PolyRaw
 * 
 * @brief Holds a polymorphic object that share a common base and whose sizes are all equivalent.
 * 
 * @tparam Base_T -- Base class.
 * @tparam N      -- Size of derived classes (in bytes).
 */
template <
   typename Base_T,
   sizet    N>
requires (requires(
   Base_T const & Base, bptr<void> const val_BPtr, sizet const Size_bytes) {
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
