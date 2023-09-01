/**
 * @author Forrest Jablonski
 */

#include "TopicSubscription.h"

#include "Ymception.h"

#include "Hsm.h"
#include "HsmSubscription.h"
#include "Topic.h"

#include <algorithm>

/**
 * 
 */
void ym::hsm::TopicSubscription::registerNewTopic(Topic const & TopicToRegister)
{
   YM_ASSERT(getTopicPtr() == nullptr, "Topic sub %s (key = %lu) has not been reset! Trying to register %s.",
      getTopicPtr()->getName(), getTopicPtr()->getKey(), TopicToRegister.getName());

   _Topic_ptr = &TopicToRegister;
}

/**
 *
 */
void ym::hsm::TopicSubscription::add(Hsm          const & HsmToAdd,
                                     SignalBase_T const   DesiredSignal)
{
   for (HsmTopicEventInfo const & HsmInfo : _hsmInfos)
   { // search through keys
      YM_ASSERT(HsmInfo.getKey() != HsmToAdd.getKey(), "Hsm %s (key = %lu) has already been added!",
         HsmToAdd.getName(), HsmToAdd.getKey());
   }

   // if we get here then the key has not yet been added

   _hsmInfos.emplace_back(HsmToAdd.getKey(), DesiredSignal);
}

/**
 * erase() is sufficient over remove() since the keys are unique.
 */
void ym::hsm::TopicSubscription::remove(Hsm const & HsmToDelete)
{
   auto const It = std::find_if(_hsmInfos.begin(), _hsmInfos.end(),
      [&HsmToDelete] (HsmTopicEventInfo const & HsmInfo) -> bool {
         return HsmInfo.getKey() == HsmToDelete.getKey();
      }
   );

   YM_ASSERT(It != _hsmInfos.end(), "Hsm %s (key = %lu) could not be found!",
      HsmToDelete.getName(), HsmToDelete.getKey());

   _hsmInfos.erase(It);
}

/**
 *
 */
void ym::hsm::TopicSubscription::reset(HsmSubContainer_T & hsmSubs_ref)
{
   for (HsmTopicEventInfo const & HsmInfo : _hsmInfos)
   {
      hsmSubs_ref[HsmInfo.getKey()].remove(*getTopicPtr());
   }

   _Topic_ptr = nullptr;
   _hsmInfos.clear(); // no need to call shrink_to_fit(), keep memory for next subscription
}
