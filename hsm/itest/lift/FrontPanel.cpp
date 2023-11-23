/**
 * @author Forrest Jablonski
 */

#include "FrontPanel.h"

#include "Elevator.h"

#include "Logger.h"

#include "../../Event.h"

#define YM_ANNOUNCE( Verbosity ) YM_HSM_ANNOUNCE( Verbosity, E )

/**
 *
 */
ym::hsm::itest::lift::FrontPanel::FrontPanel(int32 const FloorNumber, Elevator & el_ref)
   : Hsm("FrontPanel", _sm_Root, LiftDomain_T::FrontPanel),
     _FloorRequest {FloorNumber},
     _el_ref       {el_ref     } 
{
   // FloorRequest::getInstancePtr()->subscribe(this);
   // void subscribe(Hsm * h) {
   //    _subscribers.emplace_back(h);
   //    h->addSubscription(this);
   // }
   // void addSubscription(Topic * t) {
   //    _subscriptions.emplace_back(t);
   // }
   // or
   // ~FrontPanel() {
   //    FloorRequest::getInstancePtr()->unsubscribe(this);
   // }
}

/**
 *
 */
void ym::hsm::itest::lift::FrontPanel::sm_Root(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::Entry)
      {
         transTo(_sm_None);
         break;
      }

      YM_HSM_CASE(FrontPanelCommand_T::Up)
      {
         transTo(_sm_ButtonPushed_Up);
         break;
      }

      YM_HSM_CASE(FrontPanelCommand_T::Down)
      {
         transTo(_sm_ButtonPushed_Down);
         break;
      }

      YM_HSM_CASE(FrontPanelCommand_T::Reset)
      {
         transTo(_sm_None);
         break;
      }
   }
}

/**
 *
 */
void ym::hsm::itest::lift::FrontPanel::sm_None(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::Entry)
      {
         _FloorRequest.setButtonPressed(Button_T::None);
         publish(_el_ref, _FloorRequest);
         // TODO
         // this publish command can just be _elevator_Ptr->dispatch(_FloorRequest)
         // publish(_FloorRequest)
         //    there can be a topic that has an implicit constructor to take all associated
         //    messages like
         //       ElevatorComponentTopic(FloorRequest);
         //    ElevatorComponentTopic
         // publish<ElevatorComponentTopic>(_FloorRequest)
         // 
         // FloorRequest::getInstancePtr()->publish(_FloorRequest);
         break;
      }

      // root state handles Reset - essentially a no-op
   }
}

/**
 *
 */
void ym::hsm::itest::lift::FrontPanel::sm_ButtonPushed(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::EntryAction)
      {
         if (!publish(_el_ref, _FloorRequest))
         { // request not accepted (elevator maybe in an error state) - reset
            transTo(_sm_None);
         }

         break;
      }
   }
}

/**
 *
 */
void ym::hsm::itest::lift::FrontPanel::sm_ButtonPushed_Up(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::Entry)
      {
         _FloorRequest.setButtonPressed(Button_T::Up);
         break;
      }

      YM_HSM_CASE(FrontPanelCommand_T::Down)
      {
         consumeEvent();
         break;
      }

      // root state handles Up - essentially a no-op
   }
}

/**
 *
 */
void ym::hsm::itest::lift::FrontPanel::sm_ButtonPushed_Down(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::Entry)
      {
         _FloorRequest.setButtonPressed(Button_T::Down);
         break;
      }

      YM_HSM_CASE(FrontPanelCommand_T::Up)
      {
         consumeEvent();
         break;
      }

      // root state handles Down - essentially a no-op
   }
}
