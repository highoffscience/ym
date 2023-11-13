/**
 * @file    taggedmessage.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "message.h"
#include "signal.h"

#include "../common/ymdefs.h"

#include <utility>

namespace ym::hsm
{

/**
 * Intermediate class to hold type information.
 *
 * Not meant to be used dynamically.
 */
template <Signalable DerivedDomain_T,
                     DerivedDomain_T Domain,
          Signalable DerivedSignal_T,
                     DerivedSignal_T Signal>
class TaggedMessage : public Message
{
protected:
   explicit inline TaggedMessage(str const Name);

public:
   static constexpr auto getClassDomain(void) { return std::to_underlying(Domain); }
   static constexpr auto getClassSignal(void) { return std::to_underlying(Signal); }
};

/**
 * Constructor.
 */
template <Signalable DerivedDomain_T,
                     DerivedDomain_T Domain,
          Signalable DerivedSignal_T,
                     DerivedSignal_T Signal>
inline TaggedMessage<DerivedDomain_T, Domain,
                     DerivedSignal_T, Signal>::TaggedMessage(str const Name)
   : Message (Name,
              getClassDomain(),
              getClassSignal())
{
}

} // ym::hsm
