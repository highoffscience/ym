/**
 * @author Forrest Jablonski
 */

#include "MHsm.h"

#include "Updater.h"

#include "Ymception.h"

#include <cmath>

/**
 *
 */
ym::hsm::MHsm::MHsm(char const * const Name_Ptr,
                    HsmState &         rootState_ref,
                    SignalBase_T const Domain)
   : Hsm(Name_Ptr, rootState_ref, Domain),
     _areUpdatesActive {false},
     _isTimerActive    {false},
     _endTime          {     },
     _herz             {     }
{
}

/**
 *
 */
bool ym::hsm::MHsm::update(UpdatePK const &)
{
   bool msgAccepted = false; // until told otherwise

   if (areUpdatesActive())
   {
      if (isTimerActive())
      {
         if (Clock_T::now() >= _endTime)
         {
            dispatch(_s_MHsmCommandTimerExpired);
            stopTimer();
         }
      }

      _rootState_ref.dispatch(*this, _s_MHsmCommandPreUpdate);
      msgAccepted = dispatch(_s_MHsmCommandUpdate);
      _rootState_ref.dispatch(*this, _s_MHsmCommandPostUpdate);
   }

   return msgAccepted;
}

/**
 *
 */
void ym::hsm::MHsm::enableUpdates(void)
{
   if (!areUpdatesActive())
   {
      setHerz(getHerz()); // will assert if not set
   }

   _areUpdatesActive = true;
}

/**
 *
 */
void ym::hsm::MHsm::disableUpdates(void)
{
   _areUpdatesActive = false;
}

/**
 *
 */
void ym::hsm::MHsm::setHerz(float32 const Herz)
{
   YM_ASSERT(Herz > 0, "Hsm %s - Herz must be > 0! Given %lf.", getName(), Herz);

   _herz = Herz;
}

/**
 * TODO it'd be nice if I could write
 *      _endTime = Clock_T::now() + nanoize(Time_sec)
 */
void ym::hsm::MHsm::startTimer(float64 const Time_sec)
{
   _endTime = Clock_T::now() + std::chrono::nanoseconds(static_cast<int64>(Time_sec * std::nano::den));

   _isTimerActive = true;
}

/**
 *
 */
void ym::hsm::MHsm::stopTimer(void)
{
   _isTimerActive = false;
}

/**
 *
 */
void ym::hsm::MHsm::onCurrentStateExit(void)
{
   stopTimer();
}
