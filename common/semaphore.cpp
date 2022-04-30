/**
 * @author Forrest Jablonski
 */

#include "semaphore.h"

/**
 *
 */
ym::Semaphore::Semaphore(int32 const InitCount)
   : _cv             {         },
     _debtCountGuard {         },
     _count          {InitCount},
     _debtCount      {0        },
     _InitCount      {InitCount}
{
}

/**
 * Increment the semaphore.
 *
 * Unit testing has shown that _cv will notify the longest waiting thread. It appears the
 *  implementation for queueing threads is indeed a queue, however the documentation says
 *  it may wake any thread.
 */
bool ym::Semaphore::post(void)
{
   bool didIncrementDebtCount = false;

   if (_count.fetch_add(1, std::memory_order_release) < 0)
   { // just got out of debt - notify someone who was waiting
      _debtCountGuard.lock();
      ++_debtCount;
      _debtCountGuard.unlock();
      _cv.notify_one();

      didIncrementDebtCount = true;
   }

   return didIncrementDebtCount;
}

/**
 * Decrement the semaphore.
 * 
 * Will allow the calling thread to gain access to whatever resource the Semaphore is guarding
 *  if there are enough tickets (controlled by $param _count), will block otherwise.
 */
bool ym::Semaphore::wait(void)
{
   bool didDecrementDebtCount = false;

   if (_count.fetch_sub(1, std::memory_order_acquire) <= 0)
   { // in debt - wait until resource is available
      std::unique_lock<std::mutex> lock(_debtCountGuard);
      while (_debtCount == 0)
      { // guard against spurious wake-ups
         _cv.wait(lock);
      }

      --_debtCount;
      didDecrementDebtCount = true;
   }

   return didDecrementDebtCount;
}
