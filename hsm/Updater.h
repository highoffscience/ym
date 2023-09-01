/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "Nameable_NV.h"
#include "ThreadSafeProxy.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

namespace ym
{

class Logger;

namespace hsm
{

class MHsm;

class Updater : public Nameable_NV
{
public:
   virtual ~Updater(void) = default;

   inline uint32 getHerz(void) const { return _herz; }

   void addMHsmTsp(ThreadSafeProxy<MHsm> & TargetMHsmTsp_ref);

   void startTaskLoop(void);
   void stopTaskLoop (void);

   void dumpBlackboxTo(Logger & log_ref);

protected:
   typedef void (Updater::*TaskFunc_T)(void);

   explicit Updater(char const * const Name_Ptr,
                    uint32       const BlackboxSize_sec,
                    uint32       const InitHerz,
                    TaskFunc_T   const TaskFunc);

   inline void setHerz(uint32 const Herz) { _herz = Herz; }

   void addBlackboxEntry(float64 const Entry);

   bool isRunning(void) const;

   void updateMHsms(void);

private:
   uint32 /* ----------------------- */ _herz;
   std::mutex                           _targetMHsmTspsGuard;
   std::vector<ThreadSafeProxy<MHsm> *> _targetMHsmTsps;
   std::vector<float64>                 _blackbox;
   uint32 /* ----------------------- */ _nextBlackboxEntry;
   std::thread                          _taskThread;
   TaskFunc_T const /* ------------- */ _TaskFunc;
   std::atomic<bool>                    _isRunning;
   std::mutex /* ------------------- */ _blackboxGuard;
   bool                                 _hasWrapped;
};

} // hsm

} // ym
