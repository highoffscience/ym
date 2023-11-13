/**
 * @file    topic.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "../common/nameable.h"
#include "../common/publisher.h"
#include "../common/ymdefs.h"

namespace ym::hsm
{

/**
 * Not meant to be used dynamically.
 */
class Topic : public Publisher,
              public PermaNameable_NV<str>
{
protected:
   explicit Topic(str const Name);
};

} // ym::hsm
