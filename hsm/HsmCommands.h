/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "CommandDefs.h"
#include "HsmDomain.h"
#include "Signal.h"
#include "TaggedMessage.h"

namespace ym::hsm
{

/**
 *
 */
enum class HsmCommand_T : SignalBase_T
{
   Entry,        // constructor
   EntryAction,  // post-construction
   Exit,         // destructor
   UserStart     // custom signals start here
};

// ========================================================================== //
//                                   Entry                                    //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_DEFN_EMPTY_CLASS(HsmCommandEntry,
                             HsmDomain_T,  Hsm,
                             HsmCommand_T, Entry);

// ========================================================================== //
//                                 EntryAction                                //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_DEFN_EMPTY_CLASS(HsmCommandEntryAction,
                             HsmDomain_T,  Hsm,
                             HsmCommand_T, EntryAction);

// ========================================================================== //
//                                    Exit                                    //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_DEFN_EMPTY_CLASS(HsmCommandExit,
                             HsmDomain_T,  Hsm,
                             HsmCommand_T, Exit);

} // ym::hsm
