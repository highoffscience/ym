/**
 * @author Forrest Jablonski
 */

#include "Hsm.h"

#include "Logger.h"
#include "Ymception.h"

// ========================================================================== //
//                                    Hsm                                     //
// ========================================================================== //

/**
 *
 */
ym::hsm::Hsm::Hsm(char const * const Name_Ptr,
                  HsmState &         rootState_ref,
                  SignalBase_T const Domain)
   : Keyable_NV (TopicManager::getInstance().add(*this)),
     Nameable_NV(Name_Ptr                              ),
     _rootState_ref         {rootState_ref },
     _currState_ptr         {nullptr       },
     _goalState_ptr         {&rootState_ref}, // we will transition here when init() is called
     _DispatchedEvtName_ptr {nullptr       },
     _wasEventConsumed      {false         },
     _Domain                {Domain        }
     
{
}

/**
 *
 */
ym::hsm::Hsm::~Hsm(void)
{
   TopicManager::getInstance().remove(*this);
}

/**
 *
 */
void ym::hsm::Hsm::init(void)
{
   YM_ASSERT(_currState_ptr == nullptr,
      "Hsm %s - Hsm has already been initialized!", getName());

   propagateThroughInitialStates();
}

/**
 *
 */
void ym::hsm::Hsm::subscribe(SignalBase_T const   DesiredSignal,
                             Topic        const & TopicToSubscribeTo)
{
   TopicManager::getInstance().addSubscription(DesiredSignal, TopicToSubscribeTo, *this);
}

/**
 *
 */
void ym::hsm::Hsm::unsubscribe(Topic const & TopicToUnsubscribeFrom)
{
   TopicManager::getInstance().removeSubscription(TopicToUnsubscribeFrom, *this);
}

/**
 *
 */
void ym::hsm::Hsm::onCurrentStateExit(void)
{
}

/**
 *
 */
bool ym::hsm::Hsm::dispatch(Event const & E)
{
   YM_ASSERT(getDomain() == E.getMsgPtr()->getDomain(),
      "Hsm %s - Message domain (%lu) does not match Hsm domain (%lu)! (dispatch). Evt name = %s.",
      getName(), E.getMsgPtr()->getDomain(), getDomain(), E.getName());

   return dispatch(E, DispatchPK());
}

/**
 *
 */
bool ym::hsm::Hsm::dispatch(Event      const & E,
                            DispatchPK const &)
{
   YM_ASSERT(_currState_ptr != nullptr, "Hsm %s - Hsm has not been initialized!", getName());

   YM_ASSERT(E != ymDecaySignalToBaseType(HsmCommand_T::Entry) &&
             E != ymDecaySignalToBaseType(HsmCommand_T::Exit),
      "Hsm %s - dispatch() cannot be used for entry or exit events!", getName());

   YM_ASSERT(_DispatchedEvtName_ptr == nullptr,
      "Hsm %s - Event %s is already being dispatched! Wanted %s.",
      getName(), _DispatchedEvtName_ptr, E.getName());

   _DispatchedEvtName_ptr = E.getName();
   _wasEventConsumed      = false;

   HsmState * tempCurrState_ptr = _currState_ptr;

   do
   {
      tempCurrState_ptr->dispatch(*this, E);

      if (_goalState_ptr != nullptr)
      { // transition requested
         transitionToGoalState();
         consumeEvent();
      }
      else
      { // mom! dad!
         tempCurrState_ptr = tempCurrState_ptr->getParentPtr();
      }

   } while (!wasEventConsumed() && tempCurrState_ptr != nullptr);

   _DispatchedEvtName_ptr = nullptr;

   return wasEventConsumed();
}

/**
 *
 */
void ym::hsm::Hsm::transitionToGoalState(void)
{
   constexpr uint32 MaxAncestors                       = 8;
   HsmState *       ancestorsOfGoalState[MaxAncestors] = {nullptr};
   uint32           nAncestors                         = 0;
   HsmState *       goalState_ptr                      = _goalState_ptr;

   bool didTriggerOnCurrentStateExit = false;

   auto const exitSequence = [this, &didTriggerOnCurrentStateExit] (void)
   {
      _currState_ptr->dispatch(*this, _s_HsmCommandExit);
      _currState_ptr = _currState_ptr->getParentPtr();

      if (!didTriggerOnCurrentStateExit)
      {
         onCurrentStateExit();
         didTriggerOnCurrentStateExit = true;
      }

      YM_ASSERT(_goalState_ptr == nullptr,
         "Hsm %s - State transition not allowed during exit signal!", getName());
   };

   auto const populateGoalAncestors = [&] (void)
   {
      YM_ASSERT(nAncestors < MaxAncestors, // strict less than - we are 0-based
         "Hsm %s - Maximum depth of %lu has been reached!", getName(), MaxAncestors);

      ancestorsOfGoalState[nAncestors++] = goalState_ptr;
      goalState_ptr = goalState_ptr->getParentPtr();
   };

   _goalState_ptr = nullptr;

   while (_currState_ptr->getDepth() > goalState_ptr->getDepth())
   {
      exitSequence(); // modifies _currState_ptr
   }

   while (goalState_ptr->getDepth() > _currState_ptr->getDepth())
   {
      populateGoalAncestors(); // modifies goalState_ptr, ancestorsOfGoalState, nAncestors
   }

   YM_ASSERT_DEBUG(_currState_ptr->getDepth() == goalState_ptr->getDepth(),
      "Hsm %s - Nodes should have equal depth! curr-depth = %lu, goal-depth = %lu.",
      getName(), _currState_ptr->getDepth(), goalState_ptr->getDepth());

   while (_currState_ptr != goalState_ptr)
   {
      exitSequence(); // modifies _currState_ptr
      populateGoalAncestors(); // modifies goalState_ptr, ancestorsOfGoalState, nAncestors
   }

   for (uint32 i = 1; i <= nAncestors; ++i)
   {
      _goalState_ptr = nullptr;

      ancestorsOfGoalState[nAncestors - i]->dispatch(*this, _s_HsmCommandEntry);
      _currState_ptr = ancestorsOfGoalState[nAncestors - i];
   }

   propagateThroughInitialStates();
}

/**
 *
 */
void ym::hsm::Hsm::propagateThroughInitialStates(void)
{
   while (_goalState_ptr != nullptr)
   {
      YM_ASSERT(_goalState_ptr->getParentPtr() == _currState_ptr,
         "Hsm %s - Initial state must be a direct descendent of the current state!", getName());

      _currState_ptr = _goalState_ptr; // we will be in current state after the call is made
      _goalState_ptr = nullptr;

      _currState_ptr->dispatch(*this, _s_HsmCommandEntry);
   }

   _DispatchedEvtName_ptr = nullptr; // we're done with this

   dispatch(_s_HsmCommandEntryAction, DispatchPK());
}

/**
 *
 */
void ym::hsm::Hsm::transTo(HsmState & goalState_ref)
{
   YM_ASSERT(_goalState_ptr == nullptr,
      "Hsm %s - Trying to transition more than once during same event!", getName());

   _goalState_ptr = &goalState_ref;
}

// ========================================================================== //
//                                  HsmState                                  //
// ========================================================================== //

/**
 *
 */
ym::hsm::Hsm::HsmState::HsmState(HsmState *     const parent_Ptr,
                                 HsmStateFunc_T const StateFunc,
                                 char const *   const Name_Ptr)
   : Nameable_NV(Name_Ptr),
     _parent_Ptr {parent_Ptr          },
     _Depth      {getDepth(parent_Ptr)},
     _StateFunc  {StateFunc           }
{
}
