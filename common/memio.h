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

/**
 * @brief Allocates requested amount of bytes on the stack at runtime.
 *
 * - Functionally moves the stack pointer to where you want. We mimic the
 *   behaviour of variable length arrays.
 *
 * - Memory allocated by this function automatically gets freed when the
 *   embedding function goes out of scope.
 *
 * - Only allocates memory in current stack frame, so this must a macro,
 *   not an inline function.
 *
 * - [Man Page](https://man7.org/linux/man-pages/man3/alloca.3.html).
 * - [Reference Guide](https://en.cppreference.com/w/c/language/array). See section on VLA's.
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

/**
 * @brief Provides memory management resources.
 */
class MemIO
{
public:
   static bound<std::pmr::memory_resource> getNullMemResource(void) noexcept;
};

/**
 * @brief Custom base class for stack based memory resource management.
 *
 * - Users will have to register themselves using @ref setUser(). A user cannot unregister themselves,
 *   as this could lead to users without valid buffers to exist. A user trying to register to
 *   an already claimed buffer will assert.
 */
class StackBuffer_Base : public std::pmr::monotonic_buffer_resource
{
   friend class StackBufferUser;

public:
   YM_DECL_YMASSERT(Error);

protected:
   /**
    * @brief Constructor.
    *
    * @param buffer_Ptr       -- Pointer to stack allocated buffer.
    * @param BufferSize_bytes -- Size of buffer.
    */
   explicit inline StackBuffer_Base(
      bound<void> const buffer_Ptr,
      std::size_t const BufferSize_bytes) noexcept :
         std::pmr::monotonic_buffer_resource(
            buffer_Ptr,
            BufferSize_bytes,
            MemIO::getNullMemResource())
   { }

   /**
    * @brief Constructor.
    */
   explicit inline StackBuffer_Base(void) noexcept :
      std::pmr::monotonic_buffer_resource(
         MemIO::getNullMemResource())
   { }

   inline virtual ~StackBuffer_Base(void) noexcept = default;

   /// @brief Setter.
   constexpr void setUser(bound<class StackBufferUser> const user_Ptr) {
      YMASSERT(!_user_ptr, Error, YM_DAH, "StackBuffer already claimed by another user");
      _user_ptr = user_Ptr;
   }

public:
   /// @brief Getter.
   constexpr loose<class StackBufferUser const> getUserPtr(void) const noexcept {
      return _user_ptr;
   }

private:
   loose<class StackBufferUser> _user_ptr{nullptr};
};

/**
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

   // TODO
   // template <typename T>
   // inline loose<T> get(void) const noexcept {
   //    return std::start_lifetime_as<T>(_buffer.data());
   // }

private:
   std::array<std::byte, N> _buffer{};
};

/**
 * @brief Class that represents a user of a StackBuffer instance.
 */
class StackBufferUser
{
public:
   /// @brief Constructor.
   constexpr explicit StackBufferUser(bound<StackBuffer_Base> const buffer_Ptr) {
      buffer_Ptr->setUser({this, ym_AssumePtrNotNull{}});
   }
};

// ----------------------------------------------------------------------------
//                             Convenience classes
// ----------------------------------------------------------------------------

class StackStrLit : public StackBuffer_Base
{
public:
   template <std::size_t N>
   implicit inline StackStrLit(char const (&array) [N]) noexcept :
      _Ptr {array}
   { }

   implicit inline StackStrLit(rawstr const S, [[maybe_unused]] std::size_t const N, TAG) noexcept :
      _Ptr {S}
   { }

   inline auto * get(void) const noexcept { return _Ptr; }
   inline operator rawstr(void) const noexcept { return get(); }

private:
   rawstr _Ptr{nullptr};
};

/// @brief TODO
constexpr inline auto operator""_ssl(rawstr const S, std::size_t const N) {
   return StackStrLit(S, N, );
}

} // ym
