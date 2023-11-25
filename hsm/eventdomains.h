/**
 * @file    eventdomains.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "eventid.h"

#include "../common/ymdefs.h"

#include <utility>

namespace ym::hsm
{

/**
 * TODO
 */
enum class EventDomain_T : EventId_T
{
   Hsm,

   NDomains
};

/**
 * TODO
 */
constexpr auto ymInitEventSignal(EventDomain_T const Domain)
{
   return std::to_underlying(Domain) << 32_u64;
}

} // ym::hsm
