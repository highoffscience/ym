/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "../../MHsm.h"
//#include "../../ThreadSafeProxy.h"

#include "ElevatorCommands.h"
#include "FrontPanel.h"

#include <vector>

namespace ym::hsm::itest::lift
{

/**
 * I'm assuming only one person has access to this elevator for now. Elevator logic was a bit more complicated
 *  than I thought so I'll do the thinking later.
 */
class Elevator : public MHsm
{
public:
   explicit Elevator(int32 const MaxFloors, int32 const GroundFloor);
   virtual ~Elevator(void) = default;

   virtual void init(void) override;

   static inline ElevatorCommandOpen  const _s_ElevatorCommandOpen;
   static inline ElevatorCommandClose const _s_ElevatorCommandClose;

   //typedef ThreadSafeProxy<FrontPanel> FrontPanelTsp_T;

   //TODO
   //inline FrontPanelTsp_T       & getFrontPanel(int32 const Floor)       { return _floors[getIndexFromFloor(Floor)]; }
   //inline FrontPanelTsp_T const & getFrontPanel(int32 const Floor) const { return _floors[getIndexFromFloor(Floor)]; }

   inline FrontPanel       & getFrontPanel(int32 const Floor)       { return _floors[getIndexFromFloor(Floor)]; }
   inline FrontPanel const & getFrontPanel(int32 const Floor) const { return _floors[getIndexFromFloor(Floor)]; }

   inline int32 getMaxFloors  (void) const { return _MaxFloors;   }
   inline int32 getGroundFloor(void) const { return _GroundFloor; }

private:
   YM_HSM_DECL_STATE(Elevator, sm_Root,                 nullptr          );
                                                                        
   YM_HSM_DECL_STATE(Elevator, sm_Stopped,              &_sm_Root        );
   YM_HSM_DECL_STATE(Elevator, sm_Stopped_Closed,       &_sm_Stopped     );
   YM_HSM_DECL_STATE(Elevator, sm_Stopped_Open,         &_sm_Stopped     );
   YM_HSM_DECL_STATE(Elevator, sm_Stopped_Open_Opening, &_sm_Stopped_Open);
   YM_HSM_DECL_STATE(Elevator, sm_Stopped_Open_Closing, &_sm_Stopped_Open);
   YM_HSM_DECL_STATE(Elevator, sm_Stopped_Open_Opened,  &_sm_Stopped_Open);

   YM_HSM_DECL_STATE(Elevator, sm_Moving,               &_sm_Root        );
   YM_HSM_DECL_STATE(Elevator, sm_Moving_Up,            &_sm_Moving      );
   YM_HSM_DECL_STATE(Elevator, sm_Moving_Down,          &_sm_Moving      );

   int32 getIndexFromFloor(int32 const Floor, bool const DoCrossCheck = true) const;
   int32 getFloorFromIndex(int32 const Index, bool const DoCrossCheck = true) const;

   int32 const                  _MaxFloors;
   //std::vector<FrontPanelTsp_T> _floors;
   std::vector<FrontPanel> _floors;
   int32                        _destFloor;
   int32 const                  _GroundFloor;
   float64                      _currFloor;
   uint32                       _counter;
};

} // ym::hsm::itest::lift
