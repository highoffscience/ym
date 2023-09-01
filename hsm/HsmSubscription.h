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
class Topic;

/**
 * 
 */
class HsmSubscription
{
public:
   explicit inline HsmSubscription(Hsm & hsmToRegister_ref);

   inline Hsm       * getHsmPtr(void)       { return _hsm_ptr; }
   inline Hsm const * getHsmPtr(void) const { return _hsm_ptr; }

   inline KeyContainer_T const & getTopicKeys(void) const { return _topicKeys; }

   void registerNewHsm(Hsm & HsmToRegister_ref);

   void add(   Topic const & TopicToAdd   );
   void remove(Topic const & TopicToDelete);

   void reset(TopicSubContainer_T & topicSubs_ref);

private:
   Hsm *          _hsm_ptr;
   KeyContainer_T _topicKeys;
};

/**
 * Constructor.
 */
inline ym::hsm::HsmSubscription::HsmSubscription(Hsm & hsmToRegister_ref)
   : _hsm_ptr   {&hsmToRegister_ref},
     _topicKeys {                  }
{
}

} // ym::hsm
