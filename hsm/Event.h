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
 * Base class for all hsm event objects.
 */
class Event
{
public:
   /*implicit*/ inline Event(Message const & Msg);

   explicit inline Event(SignalBase_T const   Signal,
                         Message      const & Msg   );

   inline operator SignalBase_T (void) const { return _Signal; }

   inline Message const * getMsgPtr(void) const { return _Msg_Ptr;               }
   inline char    const * getName  (void) const { return getMsgPtr()->getName(); }

   template <typename DerivedTarget_T>
   inline DerivedTarget_T const * castToPtr(void) const;

   template <typename DerivedTarget_T>
   inline bool castToPtr(DerivedTarget_T const * & Msg_ptr_ref) const;

private:
   SignalBase_T    const _Signal;
   Message const * const _Msg_Ptr;
};

/**
 * Constructor.
 */
inline Event::Event(Message const & Msg)
   : Event(static_cast<SignalBase_T>(Msg), Msg)
{
}

/**
 * Constructor.
 */
inline Event::Event(SignalBase_T const   Signal,
                    Message      const & Msg)
   : _Signal  (Signal),
     _Msg_Ptr (&Msg  )
{
}

/**
 * Functionally a dynamic down cast.
 */
template <typename DerivedTarget_T>
inline DerivedTarget_T const * Event::castToPtr(void) const
{
   return getMsgPtr()->castToPtr<DerivedTarget_T>();
}

/**
 * Functionally a dynamic down cast.
 */
template <typename DerivedTarget_T>
inline bool Event::castToPtr(DerivedTarget_T const * & Msg_ptr_ref) const
{
   return getMsgPtr()->castToPtr(Msg_ptr_ref);
}

} // ym::hsm
