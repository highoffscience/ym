/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "HsmTopicEventInfo.h"
#include "Signal.h"
#include "TopicManagerDefs.h"

namespace ym::hsm
{

class Hsm;
class Topic;

/**
 * 
 */
class TopicSubscription
{
public:
   typedef std::vector<HsmTopicEventInfo> HsmTopicEventInfoContainer_T;

   explicit inline TopicSubscription(Topic const & TopicToRegister);

   inline Topic const *                        getTopicPtr(void) const { return _Topic_ptr; }
   inline HsmTopicEventInfoContainer_T const & getHsmInfos(void) const { return _hsmInfos;  }

   void registerNewTopic(Topic const & TopicToRegister);

   void add(Hsm          const & HsmToAdd,
            SignalBase_T const   DesiredSignal);

   void remove(Hsm const & HsmToDelete);

   void reset(HsmSubContainer_T & hsmSubs_ref);

private:
   Topic const *                _Topic_ptr;
   HsmTopicEventInfoContainer_T _hsmInfos;
};

/**
 * Constructor.
 */
inline TopicSubscription::TopicSubscription(Topic const & TopicToRegister)
   : _Topic_ptr {&TopicToRegister},
     _hsmInfos  {                }
{
}

} // ym::hsm
