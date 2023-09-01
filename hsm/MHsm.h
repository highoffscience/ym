/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "Hsm.h"
#include "MHsmCommands.h"
#include "PassKey.h"
#include "Signal.h"
#include "TimerDefs.h"

namespace ym::hsm
{

/**
 * Metronome Hsm
 */
class MHsm : public Hsm
{
protected:
   template <typename DerivedSignal_T>
   explicit inline MHsm(char const *    const Name_Ptr,
                        HsmState &            rootState_ref,
                        DerivedSignal_T const Domain);

   explicit MHsm(char const * const Name_Ptr,
                 HsmState &         rootState_ref,
                 SignalBase_T const Domain);

public:
   YM_PK_MAKE_W1(UpdatePK, Updater);

   bool update(UpdatePK const &);

   inline bool areUpdatesActive(void) const { return _areUpdatesActive; }
   inline bool isTimerActive(void) const { return _isTimerActive; }

   void enableUpdates (void);
   void disableUpdates(void);

   float32 getHerz(void) const { return _herz; }
   void    setHerz(float32 const Herz);

protected:
   void startTimer(float64 const Time_sec);
   void stopTimer(void);

   virtual void onCurrentStateExit(void) override;

private:
   bool          _areUpdatesActive;
   bool          _isTimerActive;
   timer::Time_T _endTime;
   float32       _herz;

   static inline MHsmCommandPreUpdate    const _s_MHsmCommandPreUpdate;
   static inline MHsmCommandUpdate       const _s_MHsmCommandUpdate;
   static inline MHsmCommandPostUpdate   const _s_MHsmCommandPostUpdate;
   static inline MHsmCommandTimerExpired const _s_MHsmCommandTimerExpired;
};

/**
 *
 */
template <typename DerivedSignal_T>
inline MHsm::MHsm(char const *    const Name_Ptr,
                  HsmState &            rootState_ref,
                  DerivedSignal_T const Domain)
   : MHsm(Name_Ptr, rootState_ref, ymDecaySignalToBaseType(Domain))
{
}

} // ym::hsm
