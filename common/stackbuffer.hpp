/**
 * @file    stackbuffer.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include <concepts>
#include <utility>

/*

struct Json : public StackBufferUser {...};

Json form;

// this version StackBuffer is agnostic to Json
auto form_buffer = StackBuffer<std::array<char, 100>>('\0');

// StackBuffer constructor will be passed Json object, CTAD will take over
auto form_buffer = form.createStackBuffer<std::array<char, 100>>('\0');
// the above form_buffer has a pointer to form.
// possible implementation:
// ---- begin ----

template <typename T, typename... Args_T>
inline auto StackBufferUser::createStackBuffer(Args_T &&... args) {
   return StackBuffer(this, ...); // creates type T in place
   // the constructor of StackBuffer will use "pointer this" to install itself
}

// ----- end -----

form_buffer.installUser(form);
// OR
form.installStackBuffer(form_buffer);

*/

namespace ym
{

/** StackBuffer
 *
 * @brief TODO
 */
struct StackBufferUser
{
   // bool installStackBuffer(FreePtr<StackBufferUser> const user_FPtr) {
   //    return (_user_fptr) ? false : (_user_fptr = user_FPtr);
   // }

   // void uninstallStackBuffer(FreePtr<StackBufferUser> const user_FPtr) {

   // }

   FreePtr<class StackBuffer> _buffer_fptr{};
};

/** StackBuffer
 *
 * @brief TODO
 */
template <
   std::derived_from<StackBufferUser> Primary_T,
   typename Buffer_T>
class StackBuffer
{
   friend class Primary_T;

private:
   template <typename... Args_T>
   constexpr explicit StackBuffer(StackBufferUser * pmary_ptr, Args_T &&... args) :
      _pmary_ptr {pmary_ptr},
      _buffer    {std::forward<Args_T>(args)...}
   {
      _pmary_ptr->install(this);
   }

   constexpr ~StackBuffer(void) {
      if (_pmary_ptr) {
         _pmary_ptr->uninstallStackBuffer(this);
      }
   }

   void installUser()

   Primary_T * _pmary_ptr;
   Buffer_T _buffer;

};

} // ym
