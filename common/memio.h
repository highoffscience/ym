/**
 * @file    memio.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include <alloca.h>

#include <memory>

namespace ym
{

/** YM_STACK_ALLOC
 *
 * @brief Allocates requested amount of bytes on the stack at runtime.
 *
 * @note Functionally moves the stack pointer to where you want. We mimic the
 *       behaviour of variable length arrays.
 *
 * @note Memory allocated by this function automatically gets freed when the
 *       embedding function goes out of scope.
 *
 * @note Only allocates memory in current stack frame, so this must a macro,
 *       not an inline function.
 *
 * @note [Man Page](https://man7.org/linux/man-pages/man3/alloca.3.html).
 *       [Reference Guide](https://en.cppreference.com/w/c/language/array). See section on VLA's.
 *
 * @param Type_      -- Type to allocate.
 * @param NElements_ -- Number of T elements to allocate room for.
 *
 * @returns Type_ * -- Pointer to newly allocated stack memory.
 */
#define YM_STACK_ALLOC(Type_, NElements_) \
   static_cast<Type_ *>(alloca((NElements_) * sizeof(Type_)))

/// @brief Global memory resource error.
YM_DECL_YMASSERT(ym_MemResourceError);

// ---------------------- class list ----------------------

// MemIO
// StackBuffer_Base
// StackBuffer
// StackBufferUser

// --------------------------------------------------------

/** MemIO
 *
 * @brief Provides memory management resources.
 */
class MemIO
{
public:
   static bound<std::pmr::memory_resource> getNullMemResource(void) noexcept;
};

/** StackBuffer_Base
 *
 * @brief Custom base class for stack based memory resource management.
 *
 * @note Users will have to register themselves using setUser(). A user cannot unregister themselves,
 *       as this could lead to users without valid buffers to exist. A user trying to register to
 *       an already claimed buffer will assert.
 */
class StackBuffer_Base : public std::pmr::monotonic_buffer_resource
{
   friend class StackBufferUser;

public:
   YM_DECL_YMASSERT(Error);

protected:
   /// @brief Constructor.
   explicit constexpr StackBuffer_Base(
      bound<void> const buffer_BPtr,
      std::size_t    const BufferSize_bytes) noexcept :
         std::pmr::monotonic_buffer_resource(
            buffer_BPtr,
            BufferSize_bytes,
            MemIO::getNullMemResource())
   { }

   /// @brief Setter.
   constexpr void setUser(BoundPtr<class StackBufferUser> const user_BPtr) {
      YMASSERT(!_user_fptr, Error, YM_DAH, "StackBuffer already claimed by another user");
      _user_fptr = user_BPtr;
   }

public:
   /// @brief Getter.
   constexpr loose<class StackBufferUser const> getUserFPtr(void) const noexcept {
      return _user_fptr;
   }

private:
   LoosePtr<class StackBufferUser> _user_fptr;
};

/** StackBuffer
 *
 * @brief Buffer to be placed on the stack and fed to a StackBufferUser.
 *
 * @note Do *not* dynamically allocate this class. The expected responsibility of this class
 *       is to not out-live the user, and thus there is only a way one communication, user to
 *       buffer, upon destruction to let the buffer know there is no user.
 */
template <std::size_t N>
requires (N > 0uz)
class StackBuffer : public StackBuffer_Base
{
   friend class StackBufferUser;

public:
   /// @brief Constructor.
   constexpr explicit StackBuffer(void) noexcept :
      StackBuffer_Base({_buffer.data(), ym_AssumePtrNotNull{}}, _buffer.size())
   { }

   static constexpr inline auto _Size = N;

private:
   std::array<std::byte, N> _buffer{};
};

/** StackBufferUser
 *
 * @brief Class that represents a user of a StackBuffer instance.
 */
class StackBufferUser
{
public:
   YM_DECL_YMASSERT(Error);

   /// @brief Constructor.
   constexpr explicit StackBufferUser(BoundPtr<StackBuffer_Base> const buffer_BPtr) {
      buffer_BPtr->setUser({this, ym_AssumePtrNotNull{}});
   }
};

} // ym
