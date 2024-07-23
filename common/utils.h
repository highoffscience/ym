/**
 * @file    utils.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "ymception.h"

namespace ym
{

/** BoundedPtr
 *
 * TODO
 */
template <typename T>
class BoundedPtr
{
public:
   YM_IMPLICIT constexpr BoundedPtr(T * const t_Ptr)
      : _t_ptr {t_Ptr}
   {
      UtilError::check(get(), "Bounded ptr cannot be null");
   }

   constexpr BoundedPtr              (std::nullptr_t) = delete;
   constexpr BoundedPtr & operator = (std::nullptr_t) = delete;

   constexpr T       * get        (void)       { return _t_ptr; }
   constexpr T const * get        (void) const { return _t_ptr; }

   constexpr T       & operator * (void)       { return *get(); }
   constexpr T const & operator * (void) const { return *get(); }

   constexpr T       * operator -> (void)       { return get(); }
   constexpr T const * operator -> (void) const { return get(); }

   YM_DECL_YMCEPT(UtilError)

private:
   T * _t_ptr;
};

} // ym
