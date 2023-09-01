/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "Signal.h"
#include "TopicManagerDefs.h"

namespace ym::hsm
{

/**
 * 
 */
class HsmTopicEventInfo
{
public:
   explicit inline HsmTopicEventInfo(Key_T        const Key,
                                     SignalBase_T const DesiredSignal);
   
   inline Key_T        getKey          (void) const { return _key;           }
   inline SignalBase_T getDesiredSignal(void) const { return _desiredSignal; }

private:
   Key_T        _key;
   SignalBase_T _desiredSignal;
};

/**
 * Constructor.
 */
inline HsmTopicEventInfo::HsmTopicEventInfo(Key_T        const Key,
                                            SignalBase_T const DesiredSignal)
   : _key           {Key          },
     _desiredSignal {DesiredSignal}
{
}

} // ym::hsm
