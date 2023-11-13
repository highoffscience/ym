/**
 * @file    publisher.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "publisher.h"

#include "subscriber.h"

#include <algorithm>

/** Publisher
 * 
 * @brief Constructor.
 */
ym::Publisher::Publisher(void)
   : _subs {}
{ }

/** subscribe
 * 
 * @brief TODO
 */
void ym::Publisher::subscribe(Subscriber * const sub_Ptr)
{
   if (sub_Ptr)
   { // valid ptr
      auto const It = std::find_if(_subs.cbegin(), _subs.cend(),
         [sub_Ptr](auto sub_wptr) {
            return !sub_Ptr->_self_sptr.owner_before(sub_wptr) &&
                   !sub_wptr           .owner_before(sub_Ptr->_self_sptr);
         }
      );

      if (It == _subs.cend())
      { // no duplicate found - add subscriber
         _subs.emplace_back(sub_Ptr->_self_sptr);
      }
   }
}

/** publish
 * 
 * @brief TODO
 */
void ym::Publisher::publish(void const * const Payload_Ptr)
{
   auto expiredSubFound = false; // until told otherwise

   for (auto sub_wptr : _subs)
   { // go through each subscriber
      if (auto sub_sptr = sub_wptr.lock(); sub_sptr)
      { // if subscriber still valid
         sub_sptr->receive(Payload_Ptr);
      }
      else
      { // subscriber no longer valid - mark for unsubscription
         expiredSubFound = true;
      }
   }

   if (expiredSubFound)
   { // erase all non-valid subscribers
      std::erase_if(_subs, [](auto sub_wptr) {
         return sub_wptr.expired();
      });
   }
}
