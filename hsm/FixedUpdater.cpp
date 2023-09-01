/**
 * @author Forrest Jablonski
 */

#include "FixedUpdater.h"

#include "TimerDefs.h"

/**
 *
 */
ym::hsm::FixedUpdater::FixedUpdater(char const * const Name_Ptr,
                                    uint32       const BlackboxSize_sec,
                                    uint32       const InitHerz)
   : Updater(Name_Ptr, BlackboxSize_sec, InitHerz, static_cast<TaskFunc_T>(&FixedUpdater::taskLoop))
{
}

/**
 *
 */
void ym::hsm::FixedUpdater::taskLoop(void)
{
   typedef timer::Tick_T<int64, std::nano> Tick_T;

   Tick_T const TickDelay(std::nano::den / getHerz()); // # of ticks per frame
   
   auto startTime = timer::Clock_T::now();
   
   while (isRunning())
   {
      Tick_T aveHeadroom(0);

      for (uint32 i = 0; i < getHerz(); ++i)
      {
         updateMHsms();
   
         auto const EndTime   = timer::Clock_T::now();
                    startTime = startTime + TickDelay;
         auto const Headroom  = startTime - EndTime;

         if (Headroom <= std::chrono::nanoseconds::zero())
         {
            startTime = EndTime;
         }
         else
         {
            std::this_thread::sleep_until(startTime);
         }

         aveHeadroom += Headroom;
      }

      aveHeadroom /= getHerz();
      addBlackboxEntry(timer::Tick_T<float64, std::milli>(aveHeadroom).count());
   }
}
