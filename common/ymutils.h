/**
 * @file    ymutils.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymassert.h"
#include "ymdefs.h"

#include "fmt/base.h"

#include <cstddef>
#include <cstdlib>
#include <exception>
#include <type_traits>

namespace ym
{

YM_DECL_YMASSERT(NullPtrError)

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
   #if (YM_CPP_STANDARD >= 20)
      requires (!std::is_member_function_pointer<T>::value)
   #endif
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
   explicit constexpr TrustedBoundedPtr(T * const t_Ptr)
      : _t_ptr {t_Ptr}
   { }

   /// @brief Compile time non-nullness checks.
   constexpr TrustedBoundedPtr              (std::nullptr_t) = delete;
   constexpr TrustedBoundedPtr & operator = (std::nullptr_t) = delete;

#if (YM_CPP_STANDARD >= 23)
   constexpr auto * get          (this auto && self) { return  self._t_ptr; }
   constexpr        operator T * (this auto && self) { return  self.get();  }
   constexpr auto & operator *   (this auto && self) { return *self.get();  }
   constexpr auto * operator ->  (this auto && self) { return  self.get();  }
#else
   constexpr T const * get                (void) const { return  _t_ptr; }
   constexpr T       * get                (void)       { return  _t_ptr; }
   
   constexpr           operator T const * (void) const { return  get();  }
   constexpr           operator T       * (void)       { return  get();  }

   constexpr T const & operator *         (void) const { return *get();  }
   constexpr T       & operator *         (void)       { return *get();  }

   constexpr T const * operator ->        (void) const { return  get();  }
   constexpr T       * operator ->        (void)       { return  get();  }
#endif

private:
   T * _t_ptr;
};

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
   implicit constexpr BoundedPtr(T * const t_Ptr)
      : TrustedBoundedPtr<T>(t_Ptr)
   {
      YMASSERT(this->get(), NullPtrError, "Bounded pointer cannot be null");
   }

   /** BoundedPtr
    * 
    * @brief Constructs a bounded pointer from TrustedBoundedPtr.
    * 
    * @param TBP -- Pointer to bind.
    * 
    * @throws Whatever BoundedPtr() throws.
    */
   implicit constexpr BoundedPtr(TrustedBoundedPtr<T> const TBP)
      : BoundedPtr(TBP.get())
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

static_assert(sizeof(BoundedPtr<int>) == sizeof(TrustedBoundedPtr<int>), "Potential slicing hazard");

/// @brief Convenience alias.
template <typename T>
using tbptr = TrustedBoundedPtr<T>;

/// @brief Convenience alias.
template <typename T>
using bptr = BoundedPtr<T>;

/// @brief Convenience alias.
using str = bptr<char const>;

/// @brief Convenience alias.
using strlit = TrustedBoundedPtr<char const>;

/// @brief Convenience user-defined literal
constexpr inline auto operator"" _str(rawstr const S, std::size_t) { return strlit(S); }

// ----------------------------------------------------------------------------

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
 * @tparam T -- Data type to cast to.
 * 
 * @param Data_Ptr -- Pointer to object(s).
 * 
 * @returns T const * -- Pointer to object(s) represented as an array of T.
 */
template <typename T>
constexpr auto const * ymCastPtrTo(void const * const Data_Ptr)
{
   return static_cast<T const *>(Data_Ptr);
}

/** ymCastPtrTo
 * 
 * @brief Non-const version of above.
 * 
 * @tparam T -- Data type to cast to.
 * 
 * @param data_Ptr -- Pointer to object(s).
 * 
 * @returns T * -- Pointer to object(s) represented as an array of T.
 */
template <typename T>
constexpr auto * ymCastPtrTo(void * const data_Ptr)
{
   return static_cast<T *>(data_Ptr);
}

/**
 * TODO
 *
 * @note Can be overloaded for other types.
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
template <typename Iterator_T,
          typename T,
          typename Compare_T>
constexpr auto ymBinarySearch(Iterator_T first,
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

} // ym

// ----------------------------------------------------------------------------

namespace fmt
{

/**
 * TODO
 */
template <>
struct formatter<ym::str> : public fmt::formatter<fmt::string_view>
{
   auto format(ym::str s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator;
};

/**
 * TODO
 */
template <>
struct formatter<ym::strlit> : public fmt::formatter<fmt::string_view>
{
   auto format(ym::strlit s, fmt::format_context & ctx_ref) const -> fmt::format_context::iterator;
};

} // fmt
