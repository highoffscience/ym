/**
 * @file    message.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

// TODO this class is now event

#pragma once

#include "signal.h"

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
concept MessageConcept = requires(T)
{
   std::is_base_of_v<class Message, T>;

   T::getClassDomain(); // lives in TaggedMessage
   T::getClassSignal();
};

/**
 * Not meant to be used dynamically.
 */
class Message : public PermaNameable_NV<str>
{
protected:
   explicit inline Message(str          const Name,
                           SignalBase_T const Domain,
                           SignalBase_T const Signal);

public:
   YM_DECL_YMCEPT(MessageError)

   inline auto getDomain(void) const { return _Domain; }
   inline auto getSignal(void) const { return _Signal; }

   inline operator SignalBase_T (void) const { return getSignal(); }

   template <MessageConcept DerivedTarget_T>
   DerivedTarget_T const * castToPtr(void) const;

   template <MessageConcept DerivedTarget_T>
   bool castToPtr(DerivedTarget_T const * & Msg_ptr_ref) const;

private:
   SignalBase_T const _Domain;
   SignalBase_T const _Signal;

   template <MessageConcept DerivedTarget_T>
   inline bool isCompatibleMsg(void) const;
};

/**
 * Constructor.
 */
inline Message::Message(str          const Name_Ptr,
                        SignalBase_T const Domain,
                        SignalBase_T const Signal)
   : PermaNameable_NV(Name_Ptr),
     _Domain {Domain},
     _Signal {Signal}
{
}

/**
 * Functionally a dynamic down cast.
 */
template <MessageConcept DerivedTarget_T>
DerivedTarget_T const * Message::castToPtr(void) const
{
   MessageError(isCompatibleMsg<DerivedTarget_T>(),
      "Casting to an incompatible message! Instance name = %s.\n"
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
template <MessageConcept DerivedTarget_T>
bool Message::castToPtr(DerivedTarget_T const * & Msg_ptr_ref) const
{
   Msg_ptr_ref = isCompatibleMsg<DerivedTarget_T>()         ?
                 static_cast<DerivedTarget_T const *>(this) :
                 nullptr;

   return Msg_ptr_ref != nullptr;
}

/**
 * Returns true if this message is an instance of DerivedTarget_T, false otherwise.
 */
template <MessageConcept DerivedTarget_T>
inline bool Message::isCompatibleMsg(void) const
{
   return getDomain() == DerivedTarget_T::getClassDomain() &&
          getSignal() == DerivedTarget_T::getClassSignal();
}

} // ym::hsm
