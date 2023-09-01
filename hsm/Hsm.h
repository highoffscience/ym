/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "Event.h"
#include "HsmCommands.h"
#include "Keyable_NV.h"
#include "Message.h"
#include "Nameable_NV.h"
#include "PassKey.h"
#include "Signal.h"
#include "Topic.h"
#include "TopicManager.h"

/**
 *
 */
#define YM_HSM_DECL_STATE(HsmClassName, StateName, Parent_Ptr)        \
   void StateName(Event const & E);                                   \
   HsmState _##StateName =                                            \
      HsmState(Parent_Ptr,                                            \
               static_cast<HsmStateFunc_T>(&HsmClassName::StateName), \
               #StateName)

/**
 *
 */
#define YM_HSM_CASE(QualifiedEnumType) case ymDecaySignalToBaseType(QualifiedEnumType):

/**
 *
 */
#define YM_HSM_ANNOUNCE( Verbosity, EventObject ) \
   YM_LOG(Verbosity, "%s::%s - %s", getName(), __func__, EventObject.getName())

namespace ym::hsm
{

/**
 * Hierarchical State Machine
 */
class Hsm : public Keyable_NV,
            public Nameable_NV
{
// ========================================================================== //
//                                  HsmState                                  //
// ========================================================================== //

protected:
   typedef void (Hsm::*HsmStateFunc_T)(Event const & E);

   /**
    *
    */
   class HsmState : public Nameable_NV
   {
   public:
      explicit HsmState(HsmState *     const parent_Ptr,
                        HsmStateFunc_T const StateFunc,
                        char const *   const Name_Ptr);

      inline void dispatch(Hsm         & hsm_ref,
                           Event const & E      ) const;

      inline HsmState * getParentPtr(void) const { return _parent_Ptr; }
      inline uint32     getDepth    (void) const { return _Depth;      }

   private:
      static constexpr uint32 getDepth(HsmState const * State_ptr);

      HsmState *     const _parent_Ptr;
      uint32         const _Depth;
      HsmStateFunc_T const _StateFunc;
   };

// ========================================================================== //
//                                    Hsm                                     //
// ========================================================================== //

protected:
   template <typename DerivedSignal_T>
   explicit inline Hsm(char const *    const Name_Ptr,
                       HsmState &            rootState_ref,
                       DerivedSignal_T const Domain);

   explicit Hsm(char const * const Name_Ptr,
                HsmState &         rootState_ref,
                SignalBase_T const Domain);

public:
   virtual ~Hsm(void);

   virtual void init(void);

   inline SignalBase_T getDomain(void) const { return _Domain; }

   template <typename DerivedSignal_T>
   inline void subscribe(DerivedSignal_T const   DesiredSignal,
                         Topic           const & TopicToSubscribeTo);

   void subscribe(SignalBase_T const   DesiredSignal,
                  Topic        const & TopicToSubscribeTo);

   void unsubscribe(Topic const & TopicToUnsubscribeFrom);

   YM_PK_MAKE_W2(DispatchPK, Hsm, TopicManager);

   TODO // this function should be protected. We need to make a dispatch function that only accepts
        // a message ref because Events store a non-owning pointer to the message and I don't trust the public
        // with that.
   bool dispatch(Event      const & E);

   bool dispatch(Event      const & E, // can also call with message ptr
                 DispatchPK const &);

protected:
   void transTo(HsmState & goalState_ref);

   virtual void onCurrentStateExit(void);

   inline bool wasEventConsumed(void) const { return _wasEventConsumed; }
   inline void consumeEvent(void) { _wasEventConsumed = true; }

   template <typename TopicMessage_T>
   void publish(TopicMessage_T const & TopicMessage);

   template <typename TopicMessage_T>
   bool publish(Hsm & hsm_ref, TopicMessage_T const & TopicMessage);

   HsmState   &       _rootState_ref;

private:
   HsmState   *       _currState_ptr;
   HsmState   *       _goalState_ptr;
   
   char const *       _DispatchedEvtName_ptr;
   bool               _wasEventConsumed;

   SignalBase_T const _Domain;

   static inline HsmCommandEntry       const _s_HsmCommandEntry;
   static inline HsmCommandEntryAction const _s_HsmCommandEntryAction;
   static inline HsmCommandExit        const _s_HsmCommandExit;
   
   void transitionToGoalState        (void);
   void propagateThroughInitialStates(void);
};

// ========================================================================== //
//                                  HsmState                                  //
// ========================================================================== //

/**
 *
 */
inline void Hsm::HsmState::dispatch(Hsm         & hsm_ref,
                                    Event const & E      ) const
{
   (hsm_ref.*_StateFunc)(E);
}

/**
 *
 */
constexpr uint32 Hsm::HsmState::getDepth(HsmState const * State_ptr)
{
   uint32 depth = 0;

   while (State_ptr != nullptr)
   {
      State_ptr = State_ptr->getParentPtr();
      depth++;
   }

   return depth;
}

// ========================================================================== //
//                                    Hsm                                     //
// ========================================================================== //

/**
 *
 */
template <typename DerivedSignal_T>
inline Hsm::Hsm(char const *    const Name_Ptr,
                HsmState &            rootState_ref,
                DerivedSignal_T const Domain)
   : Hsm(Name_Ptr, rootState_ref, ymDecaySignalToBaseType(Domain))
{
}

/**
 *
 */
template <typename DerivedSignal_T>
inline void Hsm::subscribe(DerivedSignal_T const   DesiredSignal,
                           Topic           const & TopicToSubscribeTo)
{
   subscribe(ymDecaySignalToBaseType(DesiredSignal), TopicToSubscribeTo);
}

/**
 * Yes, we need to pass in the variable twice, the first param is treated as a Topic,
 *  the second, a Message.
 */
template <typename TopicMessage_T>
void Hsm::publish(TopicMessage_T const & TopicMessage)
{
   YM_ASSERT(getDomain() == TopicMessage_T::getClassDomain(),
      "Hsm %s - Topic domain (%lu) does not match Hsm domain (%lu)! Msg name = %s. (gen pub).",
      getName(), TopicMessage_T::getClassDomain(), getDomain(), static_cast<Topic const *>(&TopicMessage)->getName());

   TopicManager::getInstance().dispatchToSubscribedHsms(TopicMessage, TopicMessage);
}

/**
 * Yes, we need to pass in the variable twice, the first param is treated as a Topic,
 *  the second, a Message.
 */
template <typename TopicMessage_T>
bool Hsm::publish(Hsm & hsm_ref, TopicMessage_T const & TopicMessage)
{
   YM_ASSERT(getDomain() == TopicMessage_T::getClassDomain(),
      "Hsm %s - Topic domain (%lu) does not match Hsm domain (%lu)! (targ pub). Msg name = %s.",
      getName(), TopicMessage_T::getClassDomain(), getDomain(), static_cast<Topic const *>(&TopicMessage)->getName());

   return TopicManager::getInstance().dispatchTo(hsm_ref, TopicMessage, TopicMessage);
}

} // ym::hsm
