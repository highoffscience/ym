/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "../../MHsm.h"

#include "TimedTrafficLightCommands.h"

namespace ym::hsm::itest::light
{

/**
 * 
 */
class TimedTrafficLight : public MHsm
{
public:
   explicit TimedTrafficLight(void);
   virtual ~TimedTrafficLight(void) = default;

   virtual void init(void) override;

   static inline TimedTrafficLightCommandChangeToRed   const _s_TimedTrafficLightCommandChangeToRed;
   static inline TimedTrafficLightCommandChangeToGreen const _s_TimedTrafficLightCommandChangeToGreen;

private:
   YM_HSM_DECL_STATE(TimedTrafficLight, sm_Root,   nullptr  );
                                                                        
   YM_HSM_DECL_STATE(TimedTrafficLight, sm_Red,    &_sm_Root);
   YM_HSM_DECL_STATE(TimedTrafficLight, sm_Yellow, &_sm_Root);
   YM_HSM_DECL_STATE(TimedTrafficLight, sm_Green,  &_sm_Root);
};

} // ym::hsm::itest::light
