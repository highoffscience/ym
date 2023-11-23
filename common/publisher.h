/**
 * @file    publisher.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include <memory>
#include <vector>

namespace ym
{

/** Publisher
 * 
 * @brief Publisher/Subscriber pattern.
 * 
 * @note Weak pointers are non-owning references that check
 *       if the object (in this case subscriber) is still active.
 */
class Publisher
{
public:
   explicit Publisher(void);

   void publish(void const * const Payload_Ptr);

   void subscribe(class Subscriber * const sub_Ptr);

private:
   std::vector<std::weak_ptr<class Subscriber>> _subs;
};

} // ym
