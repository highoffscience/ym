/**
 * @file    event.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "eventid.h"

#include "../common/nameable.h"
#include "../common/ymception.h"
#include "../common/ymdefs.h"

#include <type_traits>

namespace ym::hsm
{

/**
 * TODO
 */
template <typename T>
concept TaggedEvent_T = requires(T)
{
   std::is_base_of_v<class Event, T>;

   T::getClassName(); // lives in TaggedEvent
   T::getClassDomain();
   T::getClassSignal();
};

/**
 * Not meant to be used dynamically.
 */
class Event : public PermaNameable_NV<str>
{
protected:
   /// TODO
   explicit constexpr Event(str       const Name,
                            EventId_T const Id)
      : PermaNameable_NV(Name),
        _Id {Id}
   { }

public:
   YM_DECL_YMCEPT(EventError)

   constexpr auto getId(void) const { return _Id; }

   constexpr operator std::underlying_type_t<EventId_T> (void) const { return getId(); }

   template <TaggedEvent_T DerivedTarget_T>
   DerivedTarget_T const * castToPtr(void) const;

   template <TaggedEvent_T DerivedTarget_T>
   bool castToPtr(DerivedTarget_T const * & E_ptr_ref) const;

private:
   template <TaggedEvent_T DerivedTarget_T>
   inline bool isCompatibleMsg(void) const;

   EventId_T const _Id;
};

/**
 * Functionally a dynamic down cast.
 */
template <TaggedEvent_T DerivedTarget_T>
DerivedTarget_T const * Event::castToPtr(void) const
{
   // TODO
   EventError(isCompatibleMsg<DerivedTarget_T>(),
      "Casting to an incompatible event! Instance name = %s.\n"
      "  Instance domain = %02lu, Instance signal = %02lu.\n"
      "  Target   domain = %02lu. Target   signal = %02lu.",
      getName(),
      getDomain(), getSignal(),
      DerivedTarget_T::getClassDomain(), DerivedTarget_T::getClassSignal());

   return static_cast<DerivedTarget_T const *>(this);
}

/**
 * Functionally a dynamic down cast.
 */
template <TaggedEvent_T DerivedTarget_T>
bool Event::castToPtr(DerivedTarget_T const * & E_ptr_ref) const
{
   E_ptr_ref = isCompatibleMsg<DerivedTarget_T>()         ?
               static_cast<DerivedTarget_T const *>(this) :
               nullptr;

   return Msg_ptr_ref != nullptr;
}

/**
 * Returns true if this message is an instance of DerivedTarget_T, false otherwise.
 */
template <TaggedEvent_T DerivedTarget_T>
inline bool Event::isCompatibleMsg(void) const
{
   return getId() == DerivedTarget_T::getId();
}

} // ym::hsm
