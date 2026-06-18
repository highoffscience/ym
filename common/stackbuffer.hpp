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
auto form_buffer = StackBuffer<std::array<char, 100>>('\0');

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
   constexpr explicit StackBuffer(Args_T &&... args)
      : _buffer {std::forward<Args_T>(args)...}
   { }

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
