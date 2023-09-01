/**
 * @file    threadsafeproxy.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include <mutex>

namespace ym
{

/** ThreadSafeProxy
 *
 * @brief A thread safe wrapper.
 * 
 * @note This wrapper functionally makes operations (ie member access and function calls)
 *       thread safe. It does this by overloading the arrow operator, a recursively function
 *       until it hits a raw pointer, which is then dereferenced. We inject a proxy class
 *       before the underlying type's pointer is dereferenced that employs RAII had locks
 *       a shared mutex before the dereference, and unlocks the shared mutex at the conclusion
 *       of the statement the initial dereference is in. For example...
 * 
 *       struct A { int _i; };
 *       A a{._i = 9};
 *       ThreadSafeProxy tsp(&a);
 *       tsp->_i = 7;
 * 
 *       The arrow operator will first acquire the mutex in tsp, then the semantics of the
 *       statement will happen, then (in this example when _i has been set to 7) the
 *       destructor of the temporary monitor class will be called, releasing the mutex.
 * 
 * @tparam Underlying_T -- Type which needs thread protection.
 * @tparam Mtx_T        -- A type that satisfies @ref <https://en.cppreference.com/w/cpp/named_req/Mutex>.
 */
template <typename Underlying_T,
          typename Mtx_T = std::mutex>
class ThreadSafeProxy
{
private:
   /** Monitor
    *
    * @brief Temp proxy object that acquires lock upon creation and releases it upon destruction.
    */
   class Monitor
   {
   public:
      explicit inline Monitor(Mtx_T        * const mtx_Ptr,
                              Underlying_T * const obj_Ptr);

      YM_NO_COPY  (Monitor);
      YM_NO_ASSIGN(Monitor);

      inline Underlying_T * operator -> (void) { return _obj_Ptr; }

   private:
      std::lock_guard<Mtx_T> _lock;
      Underlying_T * const   _obj_Ptr;
   };

public:
   explicit inline ThreadSafeProxy(Underlying_T * const obj_Ptr);

   // mutex is not copyable nor movable
   YM_NO_COPY  (ThreadSafeProxy);
   YM_NO_ASSIGN(ThreadSafeProxy);

   inline auto operator -> (void);

private:
   Mtx_T                _mtx;
   Underlying_T * const _obj_Ptr;
};

/** Monitor
 *
 * @brief Constructor.
 * 
 * @tparam Underlying_T -- Type which needs thread protection.
 * @tparam Mtx_T        -- A type that satisfies @ref <https://en.cppreference.com/w/cpp/named_req/Mutex>.
 * 
 * @param mtx_Ptr -- Pointer to shared mutex.
 * @param obj_Ptr -- Pointer to wrapped object.
 */
template <typename Underlying_T,
          typename Mtx_T>
inline ThreadSafeProxy<Underlying_T, Mtx_T>::Monitor::Monitor(
   Mtx_T        * const mtx_Ptr,
   Underlying_T * const obj_Ptr)
   : _lock    {*mtx_Ptr},
     _obj_Ptr { obj_Ptr}
{
}

/** ThreadSafeProxy
 *
 * @brief Constructor.
 * 
 * @tparam Underlying_T -- Type which needs thread protection.
 * @tparam Mtx_T        -- A type that satisfies @ref <https://en.cppreference.com/w/cpp/named_req/Mutex>.
 * 
 * @param obj_Ptr -- Pointer to wrapped object.
 */
template <typename Underlying_T,
          typename Mtx_T>
inline ThreadSafeProxy<Underlying_T, Mtx_T>::ThreadSafeProxy(
   Underlying_T * const obj_Ptr)
   : _mtx     {/*default*/},
     _obj_Ptr {obj_Ptr    }
{
}

/** operator ->
 *
 * @brief Returns a temporary proxy object to allow for thread safe access.
 * 
 * @returns Monitor -- Temp proxy object.
 */
template <typename Underlying_T,
          typename Mtx_T>
inline auto ThreadSafeProxy<Underlying_T, Mtx_T>::operator -> (void)
{
   return Monitor(&_mtx, _obj_Ptr);
}

} // ym
