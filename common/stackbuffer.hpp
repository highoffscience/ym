/**
 * @file    stackbuffer.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include <memory>
#include <utility>

/*

struct Json : public StackBufferUser {...};

Json form;
auto form_buffer = form.createStackBuffer(); // form_buffer lives on stack.

*/

namespace ym
{

/** StackBuffer_Base
 *
 * @brief TODO
 */
class StackBuffer_Base : public std::pmr::monotonic_buffer_resource
{
public:
   explicit inline StackBuffer_Base(
      BoundPtr<void>                  const buffer_BPtr,
      std::size_t                     const BufferSize_bytes,
      BoundPtr<class StackBufferUser> const user_BPtr) :
         std::pmr::monotonic_buffer_resource(
            buffer_BPtr,
            BufferSize_bytes,
            std::pmr::null_memory_resource()), // TODO we should pass a custom allocator in (from ymassert)
         _user_BPtr {user_BPtr}
   { }

protected:
   BoundPtr<class StackBufferUser> const _user_BPtr;
};

/** StackBuffer
 *
 * @brief TODO
 */
template <std::size_t N>
class StackBuffer : public StackBuffer_Base
{
   friend class StackBufferUser;

private: // use StackBufferUser::createStackBuffer()
   constexpr explicit StackBuffer(BoundPtr<class StackBufferUser> const user_BPtr) noexcept;
   constexpr ~StackBuffer(void) noexcept;

private:
   std::array<std::byte, N> _buffer{};
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
   template <std::size_t N>
   constexpr auto createStackBuffer(void) {
      return StackBuffer<N>(this);
   }

protected:
   FreePtr<StackBuffer_Base> _buffer_fptr{};
};

/** StackBuffer
 *
 * @brief TODO
 */
template <std::size_t N>
constexpr StackBuffer<N>::StackBuffer(BoundPtr<StackBufferUser> const user_BPtr) noexcept :
   StackBuffer_Base(user_BPtr)
{
   _user_BPtr->_buffer_fptr = this;
}

/** ~StackBuffer
 *
 * @brief TODO
 */
template <std::size_t N>
constexpr StackBuffer<N>::~StackBuffer(void) noexcept
{
   _user_BPtr->_buffer_fptr = nullptr;
}

} // ym
