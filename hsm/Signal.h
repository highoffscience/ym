/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

namespace ym::hsm
{

typedef uint32 SignalBase_T; // must decay to uint32

/**
 * Takes an enum class and decays it to the underlying integral type.
 */
template <typename DerivedSignal_T>
constexpr SignalBase_T ymDecaySignalToBaseType(DerivedSignal_T const Signal)
{
   return static_cast<SignalBase_T>(Signal);
}

} // ym::hsm
