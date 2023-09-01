/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "Keyable_NV.h"
#include "Nameable_NV.h"

/**
 * ClassName will contain two copies of the base class Nameable_NV. Virtual inheritance fixes this problem
 *  but introduces error prone artefacts.
 */
#define YM_TPCMSG_DECL_CLASS(ClassName,                                    \
                             Domain_T, DomainValue,                        \
                             Signal_T, SignalValue)                        \
   class ClassName : public Topic,                                         \
                     public TaggedMessage<Domain_T, Domain_T::DomainValue, \
                                          Signal_T, Signal_T::SignalValue>

/**
 *
 */
#define YM_TPCMSG_CALL_BASE_CLASSES(Name) \
   Topic        (Name),                   \
   TaggedMessage(Name)

namespace ym::hsm
{

/**
 * Not meant to be used dynamically.
 */
class Topic : public Keyable_NV,
              public Nameable_NV
{
protected:
   explicit Topic(char const * const Name_Ptr);

public:
   ~Topic(void);
};

} // ym::hsm
