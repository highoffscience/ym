/**
 * @file    memio.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include <alloca.h>
#include <limits>
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
 *
 * @note Do *not* dynamically allocate this class. The expected responsibility of this class
 *       is to not out-live the user, and thus there is only a way one communication, user to
 *       buffer, upon destruction to let the buffer know there is no user.
 */
class StackBuffer_Base : public std::pmr::monotonic_buffer_resource
{
public:
   YM_DECL_YMASSERT(Error);

   /**
    * @brief Marks this buffer as claimed.
    *
    * @returns bool -- True if successfully claimed, false if there is already an assigned user.
    */
   // TODO move this to cpp file
   bool setUser(void) noexcept {
      auto success = false; // until told otherwise
      if (_nUsers == SingleUser_Unclaimed) {
         _nUsers = SingleUser_Claimed;
      }
   }

protected:
   /**
    * @brief Constructor.
    *
    * @param buffer_Ptr       -- Pointer to stack allocated buffer.
    * @param BufferSize_bytes -- Size of buffer.
    */
   explicit constexpr StackBuffer_Base(
      bound<void> const buffer_Ptr,
      std::size_t const BufferSize_bytes) noexcept :
         std::pmr::monotonic_buffer_resource(
            buffer_Ptr,
            BufferSize_bytes,
            MemIO::getNullMemResource()),
         _nUsers {std::numeric_limits<decltype(_nUsers)>::max()}
   { }

   /**
    * @brief Constructor.
    */
   explicit constexpr StackBuffer_Base(void) noexcept :
      std::pmr::monotonic_buffer_resource(
         MemIO::getNullMemResource())
   { }

   /// @brief Destructor.
   inline virtual ~StackBuffer_Base(void) noexcept = default;

private:
   static constexpr inline auto SingleUser_Unclaimed = std::numeric_limits<std::size_t>::max();
   static constexpr inline auto SingleUser_Claimed   = SingleUser_Unclaimed - 1uz;
   static constexpr inline auto MultiUser_Init       = 0uz;

   std::size_t _nUsers {SingleUser_Unclaimed};
};

/**
 * @brief Buffer to be placed on the stack and fed to a @ref StackBufferUser.
 *
 * @tparam N -- Size of buffer, in bytes.
 */
template <std::size_t N>
requires (N > 0uz)
class StackBuffer : public StackBuffer_Base
{
   friend class StackBufferUser;

public:
   /// @brief Constructor.
   inline explicit StackBuffer(void) noexcept :
      StackBuffer_Base({_buffer.data(), ym_AssumePtrNotNull{}}, _buffer.size())
   { }

   static constexpr inline auto _Size = N;

   /**
    * @brief Interprets the data as another type.
    *
    * @tparam T -- Type to interpret the bytes as.
    *
    * @returns loose<T> -- Pointer to data as the desired type.
    */
   template <typename T>
   inline loose<T> get(void) const noexcept {
      return std::start_lifetime_as<T>(_buffer.data());
   }

private:
   std::array<std::byte, N> _buffer{};
};

/**
 * @brief Class that represents a user of a StackBuffer instance.
 */
class StackBufferUser
{
public:
   /**
    * @brief Constructor.
    *
    * @param buffer_Ptr -- Buffer this user claims.
    */
   constexpr explicit StackBufferUser(bound<StackBuffer_Base> const buffer_Ptr) {
      buffer_Ptr->setUser({this, ym_AssumePtrNotNull{}}); // TODO allow for multiple users?
      // like unique_ptr vs shared_ptr?
      // FileIO is a StackBufferUser, but holding a string literal buffer should allow multiple
      //   users access because it is const data.
   }
};

// ----------------------------------------------------------------------------
//                             Convenience classes
// ----------------------------------------------------------------------------

/**
 * @brief Represents a string literal
 *
 */
class StackStrLit : public StackBuffer_Base
{
public:
   template <std::size_t N>
   implicit inline StackStrLit(char const (&Array) [N]) noexcept :
      _Ptr  {Array},
      _Size {  N  }
   { }

   implicit inline StackStrLit(
      rawstr      const Array,
      std::size_t const N) noexcept :
         _Ptr  {Array},
         _Size {  N  }
   { }

   inline virtual ~StackStrLit(void) noexcept = default;

   inline auto * get(void) const noexcept { return _Ptr; }
   inline operator rawstr(void) const noexcept { return get(); }

private:
   rawstr      const _Ptr  {nullptr};
   std::size_t const _Size {  0uz  };
};

/// @brief TODO
constexpr inline auto operator""_ssl(rawstr const S, std::size_t const N) {
   return StackStrLit(S, N);
}

} // ym
