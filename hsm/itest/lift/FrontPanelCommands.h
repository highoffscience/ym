/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "LiftDomains.h"

#include "../../HsmCommands.h"
#include "../../Signal.h"
#include "../../TaggedMessage.h"

namespace ym::hsm::itest::lift
{

/**
 *
 */
enum class FrontPanelCommand_T : SignalBase_T
{
   Up = ymDecaySignalToBaseType(HsmCommand_T::UserStart),
   Down,
   Reset
};

// ========================================================================== //
//                                     Up                                     //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_DEFN_EMPTY_CLASS(FrontPanelCommandUp,
                             LiftDomain_T,        FrontPanel,
                             FrontPanelCommand_T, Up);

// ========================================================================== //
//                                    Down                                    //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_DEFN_EMPTY_CLASS(FrontPanelCommandDown,
                             LiftDomain_T,        FrontPanel,
                             FrontPanelCommand_T, Down);

// ========================================================================== //
//                                   Reset                                    //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_DEFN_EMPTY_CLASS(FrontPanelCommandReset,
                             LiftDomain_T,        FrontPanel,
                             FrontPanelCommand_T, Reset);

} // ym::hsm::itest::lift
