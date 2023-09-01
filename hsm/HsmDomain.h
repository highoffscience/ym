/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "Signal.h"

namespace ym::hsm
{

/**
 *
 */
enum class HsmDomain_T : SignalBase_T
{
   Hsm,
   MHsm
};

} // ym::hsm
