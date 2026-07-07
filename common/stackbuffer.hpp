/**
 * @file    stackbuffer.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include "memio.h"

#include <memory>
#include <utility>

/*

struct Json : public StackBufferUser {...};

Json form;
auto form_buffer = form.createStackBuffer(); // form_buffer lives on stack.

---- see testsuite.cpp for reason why StackBuffer needs to be created first ----

auto form_buffer = StackBuffer<100>();
auto form = Json(form_buffer);
// OR
auto form = form_buffer.createUser<Json>();
// can form_buffer disappear before form?

User needs a bound pointer to a StackBuffer_Base.
StackBuffer_Base needs a free pointer to a User.

*/

namespace ym
{

/** StackBuffer_Base
 *
 * @brief TODO
 */
class StackBuffer_Base : public std::pmr::monotonic_buffer_resource
{
   friend class StackBufferUser;

protected:
   explicit inline StackBuffer_Base(
      void *      const buffer_Ptr,
      std::size_t const BufferSize_bytes) :
         std::pmr::monotonic_buffer_resource(
            buffer_Ptr,
            BufferSize_bytes,
            ymGetNullMemResource())
   { }

public:
   constexpr ~StackBuffer_Base(void) noexcept {
      _user_fptr = nullptr;
   }

protected:
   FreePtr<class StackBufferUser> _user_fptr;
};

/** StackBuffer
 *
 * @brief TODO
 *
 * @note Do *not* dynamically allocate this class.
 */
template <std::size_t N>
class StackBuffer : public StackBuffer_Base
{
   friend class StackBufferUser;

public:
   constexpr explicit StackBuffer(void) noexcept :
      StackBuffer_Base(_buffer.data(), _buffer.size())
   { }

   static constexpr inline auto _Size = N;

private:
   std::array<std::byte, N> _buffer{};
};

/** StackBufferUser
 *
 * @brief TODO
 */
class StackBufferUser
{
   template <std::size_t>
   friend class StackBuffer;

public:
   constexpr explicit StackBufferUser(BoundPtr<StackBuffer_Base> const buffer_BPtr) noexcept
      : _buffer_BPtr {buffer_BPtr}
   { }

   constexpr ~StackBufferUser(void) noexcept {
      _buffer_BPtr->_user_fptr = nullptr;
   }

protected:
   BoundPtr<StackBuffer_Base> const _buffer_BPtr;
};

} // ym
