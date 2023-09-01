/**
 * @author Forrest Jablonski
 */

#include "TimedTrafficLight.h"

#include "Logger.h"

#include "../../Event.h"

#define YM_ANNOUNCE( Verbosity ) YM_HSM_ANNOUNCE( Verbosity, E )

/**
 *
 */
ym::hsm::itest::light::TimedTrafficLight::TimedTrafficLight(void)
   : MHsm("TimedTrafficLight", _sm_Root, LightDomain_T::TimedTrafficLight)
{
}

/**
 *
 */
void ym::hsm::itest::light::TimedTrafficLight::init(void)
{
   MHsm::init();
}

/**
 *
 */
void ym::hsm::itest::light::TimedTrafficLight::sm_Root(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::Entry)
      {
         transTo(_sm_Red);
         break;
      }

      YM_HSM_CASE(TimedTrafficLightCommand_T::ChangeToRed)
      {
         // TODO set flag to stay red (or permared state)
         transTo(_sm_Red);
         break;
      }

      YM_HSM_CASE(TimedTrafficLightCommand_T::ChangeToGreen)
      {
         // TODO set flag to stay green (or permagreen state)
         transTo(_sm_Green);
         break;
      }
   }
}

/**
 * TODO auto const * const variables don't need _Ptr at end
 *      we include _ptr to indicate that the pointer itself
 *      is mutable, regardless of what it points to.
 *      auto * const variables still need _Ptr though.
 */
void ym::hsm::itest::light::TimedTrafficLight::sm_Red(Event const & E)
{
   YM_ANNOUNCE(0);

   // TODO it'd be cool to add something like
   //      handler<EntryEvent> { startTimer<std::second>(10); }
   //
   // class sm_Root : public State
   // {
   // public:
   //    explicit inline sm_Root(Name, parent_Ptr)
   //       : State {Name, parent_Ptr)
   //    {}
   // 
   //    virtual void operator () (void) override;
   // 
   // } _sm_Root;
   // 
   // typedef void (Hsm::*EventHandler_T)(Event const & E);
   //  
   // template <typename Event_T>
   // add(EventHandler_T const Handler)
   // {
   //    eventHandlers[Event_T::getClassSignal()] = Handler;
   // }
   // 
   // EventHandler_T const EventHandlers[] = {
   //    {EntryEvent::getClassSignal(), [this] (Event const * const E) {
   //       startTimer<std::second>(10);
   //    },
   //    {ExitEvent::getClassSignal(), [this] (Event const * const E) {
   //       stopTimer();
   //    },
   //    {TimerExpiredEvent::getClassSignal(), [this] (Event const * const E) {
   //       transTo(_sm_Green);
   //    }
   // };
   //

   // we can place all this in a lambda in the constructor...
   // 
   // if (E->is<EntryEvent>())
   // {
   //    startTimer<std::second>(10);
   // }
   // else if (E->is<ExitEvent>())
   // {
   //    stopTimer();
   // }
   // else if (E->is<TimerExpiredEvent>())
   // {
   //    transTo(_sm_Green);
   // }
   // 
   // dispatch(EntryEvent)
   // {
   //    startTimer<std::second>(10);
   // }
   // 
   // ym::hsm::TrafficLight::dispatch(ExitEvent)
   // {
   //    stopTimer();
   // }
   //

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::Entry)
      {
         startTimer(10); // timer automatically stops upon exit of this state
         break;
      }

      YM_HSM_CASE(MHsmCommand_T::TimerExpired)
      {
         transTo(_sm_Green);
         break;
      }
   }
}

/**
 *
 */
void ym::hsm::itest::light::TimedTrafficLight::sm_Yellow(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::Entry)
      {
         startTimer(2); // timer automatically stops upon exit of this state
         break;
      }

      YM_HSM_CASE(MHsmCommand_T::TimerExpired)
      {
         transTo(_sm_Red);
         break;
      }
   }
}

/**
 *
 */
void ym::hsm::itest::light::TimedTrafficLight::sm_Green(Event const & E)
{
   YM_ANNOUNCE(0);

   switch (E)
   {
      YM_HSM_CASE(HsmCommand_T::Entry)
      {
         startTimer(10); // timer automatically stops upon exit of this state
         break;
      }

      YM_HSM_CASE(MHsmCommand_T::TimerExpired)
      {
         transTo(_sm_Yellow);
         break;
      }
   }
}
