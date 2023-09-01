/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "LiftDomains.h"

#include "../../MHsmCommands.h"
#include "../../Signal.h"
#include "../../TaggedMessage.h"

namespace ym::hsm::itest::lift
{

/**
 * 
 */
enum class ElevatorCommand_T : SignalBase_T
{
   FloorRequest = ymDecaySignalToBaseType(MHsmCommand_T::UserStart),
   Open,
   Close
};

// ========================================================================== //
//                                    Open                                    //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_DEFN_EMPTY_CLASS(ElevatorCommandOpen,
                             LiftDomain_T,      Elevator,
                             ElevatorCommand_T, Open);

// ========================================================================== //
//                                    Close                                   //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_DEFN_EMPTY_CLASS(ElevatorCommandClose,
                             LiftDomain_T,      Elevator,
                             ElevatorCommand_T, Close);

// ========================================================================== //
//                               FloorButtonPush                              //
// ========================================================================== //

/**
 * 
 */
YM_CMD_DECL_CLASS(ElevatorCommandFloorRequest,
                  LiftDomain_T,      Elevator,
                  ElevatorCommand_T, FloorRequest)
{
public:
   explicit inline ElevatorCommandFloorRequest(int32 const FloorNumber);

   inline int32 getFloorNumber(void) const { return _FloorNumber; }

private:
   int32 const _FloorNumber;
};

/**
 * 
 */
inline ElevatorCommandFloorRequest::ElevatorCommandFloorRequest(int32 const FloorNumber)
   : TaggedMessage("ElevatorCommandFloorRequest"),
     _FloorNumber {FloorNumber}
{
}

} // ym::hsm::itest::lift
