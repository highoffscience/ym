/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "CommandDefs.h"
#include "HsmDomain.h"
#include "HsmCommands.h"
#include "Signal.h"
#include "TaggedMessage.h"

namespace ym::hsm
{

/**
 *
 */
enum class MHsmCommand_T : SignalBase_T
{
   PreUpdate = ymDecaySignalToBaseType(HsmCommand_T::UserStart),
   Update,
   PostUpdate,
   TimerExpired,
   UserStart // custom signals start here
};

// ========================================================================== //
//                                  PreUpdate                                 //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_DEFN_EMPTY_CLASS(MHsmCommandPreUpdate,
                             HsmDomain_T,   MHsm,
                             MHsmCommand_T, PreUpdate);

// ========================================================================== //
//                                   Update                                   //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_DEFN_EMPTY_CLASS(MHsmCommandUpdate,
                             HsmDomain_T,   MHsm,
                             MHsmCommand_T, Update);

// ========================================================================== //
//                                 PostUpdate                                 //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_DEFN_EMPTY_CLASS(MHsmCommandPostUpdate,
                             HsmDomain_T,   MHsm,
                             MHsmCommand_T, PostUpdate);









// ========================================================================== //
//                                TimerExpired                                //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_DEFN_EMPTY_CLASS(MHsmCommandTimerExpired,
                             HsmDomain_T,   MHsm,
                             MHsmCommand_T, TimerExpired);

} // ym::hsm
