/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "Ymception.h"

#include "Nameable_NV.h"
#include "Signal.h"

namespace ym::hsm
{

/**
 * Not meant to be used dynamically.
 */
class Message : public Nameable_NV
{
protected:
   explicit inline Message(char const * const Name_Ptr,
                           SignalBase_T const Domain,
                           SignalBase_T const Signal);

public:
   inline SignalBase_T getDomain(void) const { return _Domain; }
   inline SignalBase_T getSignal(void) const { return _Signal; }

   inline operator SignalBase_T (void) const { return getSignal(); }

   template <typename DerivedTarget_T>
   DerivedTarget_T const * castToPtr(void) const;

   template <typename DerivedTarget_T>
   bool castToPtr(DerivedTarget_T const * & Msg_ptr_ref) const;

private:
   SignalBase_T const _Domain;
   SignalBase_T const _Signal;

   template <typename DerivedTarget_T>
   inline bool isCompatibleMsg(void) const;
};

/**
 * Constructor.
 */
inline Message::Message(char const * const Name_Ptr,
                        SignalBase_T const Domain,
                        SignalBase_T const Signal)
   : Nameable_NV(Name_Ptr),
     _Domain (Domain),
     _Signal (Signal)
{
}

/**
 * Functionally a dynamic down cast.
 */
template <typename DerivedTarget_T>
DerivedTarget_T const * Message::castToPtr(void) const
{
   YM_ASSERT(isCompatibleMsg<DerivedTarget_T>(),
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
template <typename DerivedTarget_T>
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
template <typename DerivedTarget_T>
inline bool Message::isCompatibleMsg(void) const
{
   return getDomain() == DerivedTarget_T::getClassDomain() &&
          getSignal() == DerivedTarget_T::getClassSignal();
}

} // ym::hsm
