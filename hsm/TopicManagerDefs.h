/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include <vector>

namespace ym::hsm
{

typedef uint32 Key_T; // must decay to uint32

typedef std::vector<Key_T> KeyContainer_T;

typedef std::vector<class TopicSubscription> TopicSubContainer_T;
typedef std::vector<class HsmSubscription>   HsmSubContainer_T;

} // ym::hsm
