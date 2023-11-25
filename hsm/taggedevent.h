/**
 * @file    taggedevent.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "event.h"
#include "eventid.h"

#include "../common/ymdefs.h"

#include <type_traits>
#include <utility>

namespace ym::hsm
{

/**
 * Intermediate class to hold type information.
 *
 * Not meant to be used dynamically.
 */
template <auto Id, StringLiteral Name>
requires(std::is_enum_v<decltype(Id)>, "Id must be an enumeration")
class TaggedEvent : public Event
{
protected:
   explicit constexpr TaggedEvent(void)
      : Event(Name, getClassId())
   { }

public:
   static constexpr auto getClassName  (void) { return Name.value;                     }
   static constexpr auto getClassId    (void) { return std::underlying(Id);            }
   static constexpr auto getClassDomain(void) { return getClassId() >> 32_u64;         }
   static constexpr auto getClassSignal(void) { return getClassId() & 0xFFFF'FFFF_u64; }
};

} // ym::hsm
