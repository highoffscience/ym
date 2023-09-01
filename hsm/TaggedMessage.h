/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "Message.h"
#include "Signal.h"

namespace ym::hsm
{

/**
 * Intermediate class to hold type information.
 *
 * Not meant to be used dynamically.
 */
template <typename DerivedDomain_T,
                   DerivedDomain_T Domain,
          typename DerivedSignal_T,
                   DerivedSignal_T Signal>
class TaggedMessage : public Message
{
protected:
   explicit inline TaggedMessage(char const * const Name_Ptr);

public:
   static constexpr SignalBase_T getClassDomain(void) { return ymDecaySignalToBaseType(Domain); }
   static constexpr SignalBase_T getClassSignal(void) { return ymDecaySignalToBaseType(Signal); }
};

/**
 * Constructor.
 */
template <typename DerivedDomain_T,
                   DerivedDomain_T Domain,
          typename DerivedSignal_T,
                   DerivedSignal_T Signal>
inline TaggedMessage<DerivedDomain_T, Domain,
                     DerivedSignal_T, Signal>::TaggedMessage(char const * const Name_Ptr)
   : Message (Name_Ptr,
              getClassDomain(),
              getClassSignal())
{
}

} // ym::hsm
