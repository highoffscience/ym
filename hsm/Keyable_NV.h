/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "TopicManagerDefs.h"

namespace ym::hsm
{

/**
 * Not my car!
 *
 * Not meant to be used dynamically.
 */
class Keyable_NV // Non-Virtual
{
protected:
   explicit inline Keyable_NV(Key_T const Key);

public:
   inline Key_T getKey(void) const { return _Key; }

private:
   Key_T const _Key;
};

/**
 * Constructor.
 */
inline Keyable_NV::Keyable_NV(Key_T const Key)
   : _Key {Key}
{
}

} // ym::hsm
