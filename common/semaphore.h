/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace ym
{

/**
 *
 */
class Semaphore
{
public:
   explicit Semaphore(int32 const MaxCount);

   YM_NO_COPY  (Semaphore)
   YM_NO_ASSIGN(Semaphore)

   inline int32 getInitCount(void) const { return _InitCount; }

   inline int32 getCount(void) const { return _count.load(std::memory_order_relaxed); }

   bool post(void);
   bool wait(void);

private:
   std::condition_variable _cv;
   std::mutex              _debtCountGuard;
   std::atomic<int32>      _count;
   int32                   _debtCount;
   int32 const             _InitCount;
};

} // ym
