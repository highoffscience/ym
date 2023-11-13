/**
 * @file    subscriber.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "subscriber.h"

/** Subscriber
 * 
 * @brief Constructor.
 */
ym::Subscriber::Subscriber(void)
   : _self_sptr {this, [](auto){}}
{ }
