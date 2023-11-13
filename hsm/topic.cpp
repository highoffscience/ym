/**
 * @file    topic.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "topic.h"

/** Topic
 * 
 * @brief Constructor.
 */
ym::hsm::Topic::Topic(str const Name)
   : Publisher(),
     PermaNameable_NV(Name)
{
}
