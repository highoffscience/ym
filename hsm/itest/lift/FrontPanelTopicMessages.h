/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "LiftDomains.h"

#include "../../Signal.h"
#include "../../TaggedMessage.h"
#include "../../Topic.h"

namespace ym::hsm::itest::lift
{

/**
 * 
 */
enum class FrontPanelSignal_T : SignalBase_T
{
   FloorRequest
};

// ========================================================================== //
//                                FloorRequest                                //
// ========================================================================== //

/**
 * 
 */
YM_TPCMSG_DECL_CLASS(FrontPanelFloorRequest,
                     LiftDomain_T,       FrontPanel,
                     FrontPanelSignal_T, FloorRequest)
{
public:
   explicit inline FrontPanelFloorRequest(int32 const FloorNumber);

   enum Button_T { None, Up, Down };

   inline Button_T getButtonPressed(void) const { return _buttonPressed; }
   inline int32    getFloorNumber  (void) const { return _FloorNumber;   }

   inline void setButtonPressed(Button_T const ButtonPressed) { _buttonPressed = ButtonPressed; }

private:
   Button_T    _buttonPressed;
   int32 const _FloorNumber;
};

/**
 *
 */
inline FrontPanelFloorRequest::FrontPanelFloorRequest(int32 const FloorNumber)
   : YM_TPCMSG_CALL_BASE_CLASSES("FrontPanelFloorRequest"),
     _buttonPressed {Button_T::None},
     _FloorNumber   {FloorNumber   }
{
}

} // ym::hsm::itest::lift
