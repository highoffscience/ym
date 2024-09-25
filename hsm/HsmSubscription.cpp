/**
 * @author Forrest Jablonski
 */

#include "HsmSubscription.h"

#include "YmError.h"

#include "Hsm.h"
#include "Topic.h"
#include "TopicSubscription.h"

#include <algorithm>

/**
 *
 */
void ym::hsm::HsmSubscription::registerNewHsm(Hsm & HsmToRegister_ref)
{
   YM_ASSERT(getHsmPtr() == nullptr, "Hsm sub %s (key = %lu) has not been reset! Trying to register %s.",
      getHsmPtr()->getName(), getHsmPtr()->getKey(), HsmToRegister_ref.getName());

   _hsm_ptr = &HsmToRegister_ref;
}

/**
 *
 */
void ym::hsm::HsmSubscription::add(Topic const & TopicToAdd)
{
   for (Key_T const TopicKey : _topicKeys)
   { // search through keys
      YM_ASSERT(TopicKey != TopicToAdd.getKey(), "Topic %s (key = %lu) has already been added!",
         TopicToAdd.getName(), TopicToAdd.getKey());
   }

   // if we get here then the key has not yet been added

   _topicKeys.emplace_back(TopicToAdd.getKey());
}

/**
 * erase() is sufficient over remove() since the keys are unique.
 */
void ym::hsm::HsmSubscription::remove(Topic const & TopicToAdd)
{
   auto const It = std::find(_topicKeys.begin(), _topicKeys.end(), TopicToAdd.getKey());

   YM_ASSERT(It != _topicKeys.end(), "Topic %s (key = %lu) could not be found!",
      TopicToAdd.getName(), TopicToAdd.getKey());

   _topicKeys.erase(It);
}

/**
 *
 */
void ym::hsm::HsmSubscription::reset(TopicSubContainer_T & topicSubs_ref)
{
   for (Key_T const TopicKey : _topicKeys)
   {
      topicSubs_ref[TopicKey].remove(*getHsmPtr());
   }

   _hsm_ptr = nullptr;
   _topicKeys.clear(); // no need to call shrink_to_fit(), keep memory for next subscription
}
