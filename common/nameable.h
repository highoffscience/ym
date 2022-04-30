/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

namespace ym
{

/*
 * Available variants
 */
class Nameable_NV;
class CNameable_NV;

/**
 * 
 */
class Nameable_NV
{
public:
   explicit inline Nameable_NV(str const Name_Ptr)
      : _Name_ptr {Name_Ptr}
   {
   }

   inline auto getName(void) const { return _Name_ptr; }

   inline void setName(str const Name_Ptr) { _Name_ptr = Name_Ptr; }

private:
   str /* non-const */ _Name_ptr;
};

/**
 * C = Const
 */
class CNameable_NV
{
public:
   explicit inline CNameable_NV(str const Name_Ptr)
      : _Name_Ptr {Name_Ptr}
   {
   }

   inline auto getName(void) const { return _Name_Ptr; }

private:
   str const _Name_Ptr;
};

} // ym
