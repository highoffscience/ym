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

TODO move to memio.h

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
      std::size_t const BufferSize_bytes) noexcept :
         std::pmr::monotonic_buffer_resource(
            buffer_Ptr,
            BufferSize_bytes,
            ymGetNullMemResource())
   { }

   inline void setUser(FreePtr<class StackBufferUser> const user_FPtr) noexcept {
      _user_fptr = user_FPtr;
   }

   virtual void * do_allocate(size_t __bytes, size_t __alignment) override
   {
      ymLog(VF::UnitTest, "do_allocate requests {} bytes", __bytes);
      return std::pmr::monotonic_buffer_resource::do_allocate(__bytes, __alignment);
   }

public:
   inline virtual ~StackBuffer_Base(void) noexcept {
      _user_fptr = nullptr;
   }

   inline FreePtr<class StackBufferUser const> getUserFPtr(void) const noexcept {
      return _user_fptr;
   }

private:
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
      _buffer_BPtr->setUser(nullptr);
   }

protected:
   BoundPtr<StackBuffer_Base> const _buffer_BPtr;
};

} // ym
