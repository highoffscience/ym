/**
 * @author Forrest Jablonski
 */

#include "Topic.h"

#include "TopicManager.h"

/**
 * Constructor.
 */
ym::hsm::Topic::Topic(char const * const Name_Ptr)
   : Keyable_NV (TopicManager::getInstance().add(*this)),
     Nameable_NV(Name_Ptr                              )
{
}

/**
 * Destructor.
 */
ym::hsm::Topic::~Topic(void)
{
   TopicManager::getInstance().remove(*this);
}
