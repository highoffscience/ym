/**
 * @file    stackbuffer.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

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
template <typename Buffer_T>
class StackBuffer
{
public:
   template <typename... Args_T>
   constexpr explicit StackBuffer(
      BoundPtr<class StackBufferUser> const pmary_BPtr,
      Args_T &&...                          args); // TODO I want this noexcept - guarantee constructor of Buffer_T is noexcept

   constexpr ~StackBuffer(void) noexcept;

// private: TODO
   BoundPtr<class StackBufferUser> const _pmary_BPtr;
   Buffer_T                              _buffer;
};

/** StackBufferUser
 *
 * @brief TODO
 */
class StackBufferUser
{
public:
   constexpr explicit StackBufferUser(void) noexcept = default;

   /** createStackBuffer
    *
    * @brief TODO
    */
   template <
      typename    Buffer_T,
      typename... Args_T>
   constexpr auto createStackBuffer(Args_T &&... args) {
      return StackBuffer<Buffer_T>(this, std::forward<Args_T>(args)...);
   }

// protected: TODO
   FreePtr<class StackBuffer> _buffer_fptr{}; // TODO I need template type here for StackBuffer
};

/** StackBuffer
 *
 * @brief TODO
 */
template <typename    Buffer_T>
template <typename... Args_T>
constexpr StackBuffer<Buffer_T>::StackBuffer(
   BoundPtr<StackBufferUser> const pmary_BPtr,
   Args_T &&...                    args) :
      _pmary_BPtr {         pmary_BPtr          },
      _buffer     {std::forward<Args_T>(args)...}
{
   _pmary_BPtr->_buffer_fptr = this;
}

/** ~StackBuffer
 *
 * @brief TODO
 */
template <typename Buffer_T>
constexpr StackBuffer<Buffer_T>::~StackBuffer(void) noexcept
{
   _pmary_BPtr->_buffer_fptr = nullptr;
}

} // ym
