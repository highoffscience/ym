/**
 * @author Forrest Jablonski
 */

#include "VariableUpdater.h"

#include "TimerDefs.h"

/**
 *
 */
ym::hsm::VariableUpdater::VariableUpdater(char const * const Name_Ptr,
                                          uint32       const BlackboxSize_sec,
                                          uint32       const InitHerz)
   : Updater(Name_Ptr, BlackboxSize_sec, InitHerz, static_cast<TaskFunc_T>(&VariableUpdater::taskLoop))
{
}

/**
 *
 */
void ym::hsm::VariableUpdater::taskLoop(void)
{
   //// TODO should be part of Timer class
   //typedef std::chrono::duration<int64, std::nano> Tick_T;
   //typedef std::chrono::high_resolution_clock      Clock_T;
   //
   //Tick_T tickDelay(std::nano::den / getHerz()); // # of ticks per frame
   //
   //auto startTime = Clock_T::now();
   //
   //Tick_T aveHeadroom(0);
   //
   //while (isRunning())
   //{
   //   for (uint32 i = 0; i < getHerz(); ++i)
   //   {
   //      updateMHsms();
   //
   //      auto const EndTime   = Clock_T::now();
   //                 startTime = startTime + tickDelay;
   //      auto const Headroom  = startTime - EndTime;
   //
   //      if (Headroom < std::chrono::nanoseconds::zero())
   //      {
   //         startTime = EndTime;
   //      }
   //      else
   //      {
   //         std::this_thread::sleep_until(startTime);
   //      }
   //
   //      aveHeadroom += Headroom;
   //   }
   //
   //   aveHeadroom /= getHerz();
   //   addBlackboxEntry(aveHeadroom.count());
   //}
}
