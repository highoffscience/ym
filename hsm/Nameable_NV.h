/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "Ymerror.h"

namespace ym::hsm
{

/**
 * Not meant to be used dynamically.
 */
class Nameable_NV // Non-Virtual
{
protected:
   explicit inline Nameable_NV(char const * const Name_Ptr);

public:
   inline char const * getName(void) const { return _Name_Ptr; }

private:
   char const * const _Name_Ptr;
};

/**
 * Constructor.
 */
inline Nameable_NV::Nameable_NV(char const * const Name_Ptr)
   : _Name_Ptr {Name_Ptr}
{
   YM_ASSERT(getName() != nullptr, "Name cannot be null!");
}

} // ym::hsm
