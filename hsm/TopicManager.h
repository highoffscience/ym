/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "Signal.h"
#include "TopicManagerDefs.h"

namespace ym::hsm
{

class Hsm;
class Message;
class Topic;

/**
 *
 */
class TopicManager
{
private:
   explicit TopicManager(uint32 const ExpectedNSubs);

public:
   static TopicManager & getInstance(void);

   void dispatchToSubscribedHsms(Topic   const & TopicToDispatchTo,
                                 Message const & MessageToDispatch);

   bool dispatchTo(Hsm           & targetHsm_ref,
                   Topic   const & TopicToDispatchTo,
                   Message const & MessageToDispatchTo);

   Key_T add(Topic const & TopicToRegister  );
   Key_T add(Hsm         & hsmToRegister_ref);

   void remove(Topic const & TopicToBeRemoved);
   void remove(Hsm   const & HsmToBeRemoved  );

   void addSubscription(SignalBase_T const   DesiredSignal,
                        Topic        const & TopicToSubscribeTo,
                        Hsm          const & SubscribingHsm);

   void removeSubscription(Topic const & TopicToUnsubscribeFrom,
                           Hsm   const & UnsubscribingHsm);
   
private:
   TopicSubContainer_T _topicSubs;
   HsmSubContainer_T   _hsmSubs;

   KeyContainer_T      _vacantTopicKeys;
   KeyContainer_T      _vacantHsmKeys;
};

} // ym::hsm