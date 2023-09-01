/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "../../Hsm.h"

#include "FrontPanelCommands.h"
#include "FrontPanelTopicMessages.h"

namespace ym::hsm::itest::lift
{

class Elevator;

/**
 *
 */
class FrontPanel : public Hsm
{
public:
   explicit FrontPanel(int32 const FloorNumber, Elevator & el_ref);
   virtual ~FrontPanel(void) = default;

   static inline FrontPanelCommandUp    const _s_FrontPanelCommandUp;
   static inline FrontPanelCommandDown  const _s_FrontPanelCommandDown;
   static inline FrontPanelCommandReset const _s_FrontPanelCommandReset;

   typedef FrontPanelFloorRequest::Button_T Button_T;

   inline FrontPanelFloorRequest const & getFloorRequest(void) const { return _FloorRequest; }

private:
   YM_HSM_DECL_STATE(FrontPanel, sm_Root,              nullptr);
                                                       
   YM_HSM_DECL_STATE(FrontPanel, sm_None,              &_sm_Root);
   YM_HSM_DECL_STATE(FrontPanel, sm_ButtonPushed,      &_sm_Root);

   YM_HSM_DECL_STATE(FrontPanel, sm_ButtonPushed_Up,   &_sm_ButtonPushed);
   YM_HSM_DECL_STATE(FrontPanel, sm_ButtonPushed_Down, &_sm_ButtonPushed);

   FrontPanelFloorRequest _FloorRequest;
   Elevator &             _el_ref;
};

} // ym::hsm::itest::lift
