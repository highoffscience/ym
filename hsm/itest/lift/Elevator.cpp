/**
 * @author Forrest Jablonski
 */

#include "Elevator.h"

#include "Logger.h"
#include "Ymerror.h"

#include "../../Event.h"

#include <cmath>

#define YM_ANNOUNCE( Verbosity ) YM_HSM_ANNOUNCE( Verbosity, E )

/**
 *
 */
ym::hsm::itest::lift::Elevator::Elevator(int32 const MaxFloors, int32 const GroundFloor)
   : MHsm("Elevator", _sm_Root, LiftDomain_T::Elevator),
     _MaxFloors   {MaxFloors  },
     _floors      {           },
     _destFloor   {GroundFloor},
     _GroundFloor {GroundFloor},
     _currFloor   (GroundFloor),
     _counter     {0          }
     
{
   YM_ASSERT(getMaxFloors() > 1, "Hsm %s - Why would you have an elevator with %ld floor(s)?",
      getName(), getMaxFloors());

   YM_ASSERT(getGroundFloor() > 0, "Hsm %s - Ground floor must be > 0! Humans are 1-based animals.", getName());

   YM_ASSERT(getGroundFloor() <= getMaxFloors(),
      "Hsm %s - Ground floor (proposed = %ld) cannot be greater than maximum number of floors (%ld)!",
      getName(), getGroundFloor(), getMaxFloors());

   _floors.reserve(getMaxFloors());

   for (int32 i = 0; i < getMaxFloors(); ++i)
   {
      //TODO
      _floors.emplace_back(getFloorFromIndex(i), *this);

      //TODO
      subscribe(ElevatorCommand_T::FloorRequest, _floors.back().getFloorRequest());
   }
}

/**
 *
 */
void ym::hsm::itest::lift::Elevator::init(void)
{
   MHsm::init();

   for (auto & floor_ref : _floors)
   {
      //TODO
      //floor_ref.init();
   }
}

/**
 *
 */
auto ym::hsm::itest::lift::Elevator::getIndexFromFloor(int32 const Floor,
                                                       bool  const DoCrossCheck) const -> int32
{
   YM_ASSERT_DEBUG(Floor != 0, "Hsm %s - Floor cannot be 0!", getName());

   YM_ASSERT_DEBUG(Floor > -getGroundFloor(), "Hsm %s - Floor %ld too low! Ground = %ld.",
      getName(), Floor, getGroundFloor());

   YM_ASSERT_DEBUG(Floor <= getMaxFloors() - getGroundFloor() + 1,
      "Hsm %s - Floor %ld too high! MaxFloors = %ld. Ground = %ld.",
      getName(), Floor, getMaxFloors(), getGroundFloor());

   int32 const Index = (Floor < 0) ? Floor + getGroundFloor() - 1 :
                                     Floor + getGroundFloor() - 2 ;

   if (DoCrossCheck)
   {
      YM_ASSERT_DEBUG(Floor == getFloorFromIndex(Index, false),
         "Hsm %s - Cross check failed on index = %ld, floor = %ld!",
         getName(), Floor, Index);
   }

   return Index;
}

/**
 *
 */
auto ym::hsm::itest::lift::Elevator::getFloorFromIndex(int32 const Index,
                                                       bool  const DoCrossCheck) const -> int32
{
   YM_ASSERT_DEBUG(Index >= 0 && Index < getMaxFloors(), "Hsm %s - Index %ld must be in [0, %ld)!",
      getName(), Index, getMaxFloors());

   int32 const Floor = (Index + 1 < getGroundFloor()) ? 1 - getGroundFloor() + Index :
                                                        2 - getGroundFloor() + Index ;

   if (DoCrossCheck)
   {
      YM_ASSERT_DEBUG(Index == getIndexFromFloor(Floor, false),
         "Hsm %s - Cross check failed on floor = %ld, index = %ld!",
         getName(), Index, Floor);
   }

   return Floor;
}

/**
 *
 */
void ym::hsm::itest::lift::Elevator::sm_Root(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::Entry)
      {
         transTo(_sm_Stopped);
         break;
      }
   }
}

/**
 *
 */
void ym::hsm::itest::lift::Elevator::sm_Stopped(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::Entry)
      {
         transTo(_sm_Stopped_Closed);
         break;
      }
   }
}

/**
 *
 */
void ym::hsm::itest::lift::Elevator::sm_Stopped_Closed(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::EntryAction)
      {
         if (_currFloor != _destFloor)
         {
            transTo(_sm_Moving);
         }
         
         consumeEvent();
         break;
      }

      YM_HSM_CASE(ElevatorCommand_T::FloorRequest)
      {
         auto const E_Ptr = E.castToPtr<FrontPanelFloorRequest>();

         if (E_Ptr->getButtonPressed() != FrontPanelFloorRequest::Button_T::None)
         {
            int32 const FloorNumber = E_Ptr->getFloorNumber();

            if (_currFloor == FloorNumber)
            {
               transTo(_sm_Stopped_Open_Opening);
            }
            else
            {
               _destFloor = FloorNumber;
               transTo(_sm_Moving);
            }
         }

         consumeEvent();
         break;
      }
   }
}

/**
 *
 */
void ym::hsm::itest::lift::Elevator::sm_Stopped_Open(Event const & E)
{
   YM_ANNOUNCE(0);
}

/**
 *
 */
void ym::hsm::itest::lift::Elevator::sm_Stopped_Open_Closing(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::Entry)
      {
         startTimer(2.0);
         break;
      }

      YM_HSM_CASE(HsmCommand_T::Exit)
      {
         stopTimer();
         break;
      }

      YM_HSM_CASE(MHsmCommand_T::TimerExpired)
      {
         transTo(_sm_Stopped_Closed);
         break;
      }
   }
}

/**
 *
 */
void ym::hsm::itest::lift::Elevator::sm_Stopped_Open_Opening(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::Entry)
      {
         startTimer(2.0);
         break;
      }

      YM_HSM_CASE(HsmCommand_T::Exit)
      {
         stopTimer();
         break;
      }

      YM_HSM_CASE(MHsmCommand_T::TimerExpired)
      {
         transTo(_sm_Stopped_Open_Opened);
         break;
      }
   }
}

/**
 *
 */
void ym::hsm::itest::lift::Elevator::sm_Stopped_Open_Opened(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::Entry)
      {
         startTimer(5.0); // door closes in 5 seconds of inactivity
         break;
      }

      YM_HSM_CASE(HsmCommand_T::Exit)
      {
         stopTimer();
         break;
      }

      YM_HSM_CASE(MHsmCommand_T::TimerExpired)
      {
         transTo(_sm_Stopped_Open_Closing);
         break;
      }

      YM_HSM_CASE(ElevatorCommand_T::FloorRequest)
      {
         int32 const FloorNumber = E.castToPtr<FrontPanelFloorRequest>()->getFloorNumber();

         if (_currFloor != FloorNumber)
         {
            _destFloor = FloorNumber;
            transTo(_sm_Stopped_Open_Closing);
         }

         consumeEvent();
         break;
      }
   }
}

/**
 *
 */
void ym::hsm::itest::lift::Elevator::sm_Moving(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::Entry)
      {
         if (_destFloor > _currFloor)
         {
            transTo(_sm_Moving_Up);
         }
         else if (_destFloor < _currFloor)
         {
            transTo(_sm_Moving_Down);
         }
         else
         {
            YM_ASSERT(false, "Hsm %s - Elevator already on floor %lf!", getName(), _currFloor);
         }

         constexpr float64 SecondsPerFloor = 2;
         float64 const NFloorsToGo = std::abs(_currFloor - _destFloor);
         float64 const DelayTime_sec = NFloorsToGo * SecondsPerFloor;

         startTimer(DelayTime_sec);

         YM_LOG(0, "Hsm %s - Simulating %lf(s) time delay.", getName(), DelayTime_sec);
         break;
      }

      YM_HSM_CASE(HsmCommand_T::Exit)
      {
         stopTimer();
         break;
      }

      YM_HSM_CASE(MHsmCommand_T::TimerExpired)
      {
         transTo(_sm_Stopped_Open_Opened);
         break;
      }
   }
}

/**
 *
 */
void ym::hsm::itest::lift::Elevator::sm_Moving_Up(Event const & E)
{
   YM_ANNOUNCE(0);
}

/**
 *
 */
void ym::hsm::itest::lift::Elevator::sm_Moving_Down(Event const & E)
{
   YM_ANNOUNCE(0);
}
