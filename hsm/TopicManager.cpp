/**
 * @author Forrest Jablonski
 */

#include "TopicManager.h"

#include "YmError.h"

#include "Event.h"
#include "Hsm.h"
#include "HsmSubscription.h"
#include "Message.h"
#include "Topic.h"
#include "TopicSubscription.h"

#include <algorithm>

/**
 *
 */
ym::hsm::TopicManager::TopicManager(uint32 const ExpectedNSubs)
   : _topicSubs       {},
     _hsmSubs         {},
     _vacantTopicKeys {},
     _vacantHsmKeys   {}
{
   _topicSubs.reserve(ExpectedNSubs);
   _hsmSubs  .reserve(ExpectedNSubs);
}

/**
 *
 */
auto ym::hsm::TopicManager::getInstance(void) -> TopicManager &
{
   constexpr uint32 ExpectedNSubs = 128;

   static TopicManager s_instance(ExpectedNSubs);

   return s_instance;
}

/**
 *
 */
void ym::hsm::TopicManager::dispatchToSubscribedHsms(Topic   const & TopicToDispatchTo,
                                                     Message const & MessageToDispatchTo)
{
   for (HsmTopicEventInfo const & HsmInfo : _topicSubs[TopicToDispatchTo.getKey()].getHsmInfos())
   {
      Event updateEvent(HsmInfo.getDesiredSignal(), MessageToDispatchTo);
      _hsmSubs[HsmInfo.getKey()].getHsmPtr()->dispatch(updateEvent, Hsm::DispatchPK()); // we don't care about the reply
   }
}

/**
 *
 */
bool ym::hsm::TopicManager::dispatchTo(Hsm           & targetHsm_ref,
                                       Topic   const & TopicToDispatchTo,
                                       Message const & MessageToDispatchTo)
{
   auto const & HsmInfos = _topicSubs[TopicToDispatchTo.getKey()].getHsmInfos();

   for (auto const & HsmInfo : HsmInfos)
   {
      if (HsmInfo.getKey() == targetHsm_ref.getKey())
      {
         Event updateEvent(HsmInfo.getDesiredSignal(), MessageToDispatchTo);
         return _hsmSubs[HsmInfo.getKey()].getHsmPtr()->dispatch(updateEvent,
                                                                 Hsm::DispatchPK()); // we don't care about the reply
      }
   }

   YM_ASSERT_WRV(false, false, "Hsm %s is not registered for topic %s! Wanted to publish %s.",
      targetHsm_ref.getName(), TopicToDispatchTo.getName(), MessageToDispatchTo.getName());
}

/**
 * Do NOT access members of TopicToRegister_Ref! It is still being constructed.
 */
auto ym::hsm::TopicManager::add(Topic const & TopicToRegister) -> Key_T
{
   for (auto const & TopicSub : _topicSubs)
   {
      YM_ASSERT(TopicSub.getTopicPtr() != &TopicToRegister,
         "Topic %s (key = %lu) already registerd!",
         TopicSub.getTopicPtr()->getName(),
         TopicSub.getTopicPtr()->getKey());
   }

   if (_vacantTopicKeys.empty())
   { // make new slot for incoming key
      _topicSubs.emplace_back(TopicToRegister);
      return static_cast<Key_T>(_topicSubs.size() - 1U);
   }
   else
   { // vacant slot available for incoming key
      Key_T const Key = _vacantTopicKeys.back();
      _vacantTopicKeys.pop_back();
      _topicSubs[Key].registerNewTopic(TopicToRegister);
      return Key;
   }
}

/**
 * Do NOT access members of hsmToRegister_ref! It is still being constructed.
 */
auto ym::hsm::TopicManager::add(Hsm & hsmToRegister_ref) -> Key_T
{
   for (auto const & HsmSub : _hsmSubs)
   {
      YM_ASSERT(HsmSub.getHsmPtr() != &hsmToRegister_ref,
         "Hsm %s (key = %lu) already registerd!",
         HsmSub.getHsmPtr()->getName(),
         HsmSub.getHsmPtr()->getKey());
   }

   if (_vacantHsmKeys.empty())
   {
      _hsmSubs.emplace_back(hsmToRegister_ref);
      return static_cast<Key_T>(_hsmSubs.size() - 1U);
   }
   else
   {
      Key_T const Key = _vacantHsmKeys.back();
      _vacantHsmKeys.pop_back();
      _hsmSubs[Key].registerNewHsm(hsmToRegister_ref);
      return Key;
   }
}

/**
 *
 */
void ym::hsm::TopicManager::remove(Topic const & TopicToBeRemoved)
{
   _topicSubs[TopicToBeRemoved.getKey()].reset(_hsmSubs);
   _vacantTopicKeys.emplace_back(TopicToBeRemoved.getKey());
}

/**
 *
 */
void ym::hsm::TopicManager::remove(Hsm const & HsmToBeRemoved)
{
   _hsmSubs[HsmToBeRemoved.getKey()].reset(_topicSubs);
   _vacantHsmKeys.emplace_back(HsmToBeRemoved.getKey());
}

/**
 *
 */
void ym::hsm::TopicManager::addSubscription(SignalBase_T const   DesiredSignal,
                                            Topic        const & TopicToSubscribeTo,
                                            Hsm          const & SubscribingHsm)
{
   _topicSubs[TopicToSubscribeTo.getKey()].add(SubscribingHsm, DesiredSignal);
   _hsmSubs[SubscribingHsm.getKey()].add(TopicToSubscribeTo);
}

/**
 *
 */
void ym::hsm::TopicManager::removeSubscription(Topic const & TopicToUnsubscribeFrom,
                                               Hsm   const & UnsubscribingHsm)
{
   _topicSubs[TopicToUnsubscribeFrom.getKey()].remove(UnsubscribingHsm);
   _hsmSubs[UnsubscribingHsm.getKey()].remove(TopicToUnsubscribeFrom);
}
