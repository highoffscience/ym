/**
 * @author Forrest Jablonski
 */

#include "Updater.h"

#include "MHsm.h"

#include "Logger.h"
#include "YmError.h"

#include <chrono>

/**
 *
 */
ym::hsm::Updater::Updater(char const * const Name_Ptr,
                          uint32       const BlackboxSize_sec,
                          uint32       const InitHerz,
                          TaskFunc_T   const TaskFunc)
   : Nameable_NV(Name_Ptr),
     _herz                {InitHerz       },
     _targetMHsmTspsGuard {               },
     _targetMHsmTsps      {               },
     _blackbox            (BlackboxSize_sec * InitHerz, 0),
     _nextBlackboxEntry   {0              },
     _taskThread          {               },
     _TaskFunc            {TaskFunc       },
     _isRunning           {false          },
     _blackboxGuard       {               },
     _hasWrapped          {false          }
{
   YM_ASSERT(getHerz() > 0, "Updater %s - Needs a herz value > 0!", getName());
   YM_ASSERT(_blackbox.size() > 0, "Updater %s - Needs a blackbox size > 0!", getName());
}

/**
 *
 */
void ym::hsm::Updater::addMHsmTsp(ThreadSafeProxy<MHsm> & TargetMHsmTsp_ref)
{
   YM_LOCK_MTX(_targetMHsmTspsGuard);

   // TODO

   // for (auto const * const MHsmTsp_ref : _targetMHsmTsps)
   // {
   //    if (MHsmTsp_ref->getObjPtr() == TargetMHsmTsp_ref.getObjPtr())
   //    {
   //       return;
   //    }
   // }
   // 
   // _targetMHsmTsps.emplace_back(&TargetMHsmTsp_ref);
}

/**
 *
 */
void ym::hsm::Updater::startTaskLoop(void)
{
   _isRunning.store(true, std::memory_order_relaxed);

   _taskThread = std::thread(_TaskFunc, this); // starts the thread
}

/**
 *
 */
void ym::hsm::Updater::stopTaskLoop(void)
{
   _isRunning.store(false, std::memory_order_relaxed);

   _taskThread.join();
}

/**
 *
 */
void ym::hsm::Updater::addBlackboxEntry(float64 const Entry)
{
   YM_LOCK_MTX(_blackboxGuard);

   _blackbox[_nextBlackboxEntry] = Entry;

   _nextBlackboxEntry = (_nextBlackboxEntry + 1) % _blackbox.size();
   _hasWrapped        = (_nextBlackboxEntry == 0);
}

/**
 *
 */
void ym::hsm::Updater::dumpBlackboxTo(Logger & log_ref)
{
   YM_LOCK_MTX(_blackboxGuard);

   uint32 const StartIndex = (_hasWrapped) ? _nextBlackboxEntry : 0;
   uint32 const NEntries   = (_hasWrapped) ? static_cast<uint32>(_blackbox.size()) : _nextBlackboxEntry;

   for (uint32 i = StartIndex; i < NEntries; ++i)
   {
      log_ref.printf(0, "%lf", _blackbox[i]);
   }
}

/**
 *
 */
bool ym::hsm::Updater::isRunning(void) const
{
   return _isRunning.load(std::memory_order_relaxed);
}

/**
 *
 */
void ym::hsm::Updater::updateMHsms(void)
{
   YM_LOCK_MTX(_targetMHsmTspsGuard);

   for (auto const targetMHsm_Ptr : _targetMHsmTsps)
   {
      (*targetMHsm_Ptr)->update(MHsm::UpdatePK());
   }
}
