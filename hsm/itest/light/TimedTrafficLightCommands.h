/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "LightDomains.h"

#include "../../MHsmCommands.h"
#include "../../Signal.h"
#include "../../TaggedMessage.h"

namespace ym::hsm::itest::light
{

/**
 * 
 */
enum class TimedTrafficLightCommand_T : SignalBase_T
{
   ChangeToRed = ymDecaySignalToBaseType(MHsmCommand_T::UserStart),
   ChangeToGreen
};

// ========================================================================== //
//                                ChangeToRed                                 //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_DEFN_EMPTY_CLASS(TimedTrafficLightCommandChangeToRed,
                             LightDomain_T,              TimedTrafficLight,
                             TimedTrafficLightCommand_T, ChangeToRed);

// ========================================================================== //
//                               ChangeToGreen                                //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_DEFN_EMPTY_CLASS(TimedTrafficLightCommandChangeToGreen,
                             LightDomain_T,              TimedTrafficLight,
                             TimedTrafficLightCommand_T, ChangeToGreen);

} // ym::hsm::itest::light
