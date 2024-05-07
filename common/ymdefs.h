/**
 * @file    ymdefs.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 *
 * @note This file should be included in every file of the project. It provides
 *       standard declarations to be shared throughout.
 * 
 * @note Macros start with "YM_".
 * 
 * @note Macros used for a particular purpose start with "YM_SPECIAL_".
 */

#pragma once

#include "primitivedefs.h"

#include "ymception.h"

namespace ym
{

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
class BoundedPtr
{
public:
   YM_IMPLICIT constexpr BoundedPtr(T * const t_Ptr)
      : _t_ptr {t_Ptr}
   {
      BPtrError::check(get(), "Bounded ptr cannot be null");
   }

   constexpr BoundedPtr              (std::nullptr_t) = delete;
   constexpr BoundedPtr & operator = (std::nullptr_t) = delete;

   constexpr T       * get        (void)       { return _t_ptr; }
   constexpr T const * get        (void) const { return _t_ptr; }

   constexpr T       & operator * (void)       { return *get(); }
   constexpr T const & operator * (void) const { return *get(); }

   constexpr T       * operator -> (void)       { return get(); }
   constexpr T const * operator -> (void) const { return get(); }

   YM_DECL_YMCEPT(BPtrError)

private:
   T * _t_ptr;
};

template <typename T>
using bptr = BoundedPtr<T>;

using str = bptr<char>;

} // ym
