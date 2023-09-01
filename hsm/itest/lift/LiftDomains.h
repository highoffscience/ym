/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "../../Signal.h"

namespace ym::hsm::itest::lift
{
   /**
    *
    */
   enum class LiftDomain_T : SignalBase_T
   {
      Elevator,
      FrontPanel
   };
} // ym::hsm::itest::lift
