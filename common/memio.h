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
#include <type_traits>
#include <utility>

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

// -------------------------------- class list --------------------------------

// MemIO
// StackBuffer_Base
// StackBuffer
// StackBufferUser

// ----------------------------------------------------------------------------

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
 * - Users will have to register themselves using @ref addUser(). A user cannot unregister themselves,
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

   bool addUser(void) noexcept;

protected:
   /// @brief CV qualifier of storage pointed to by buffer.
   enum class Qualifier_T
   {
      NonConst,
      Const
   };

   /// @brief Tag to disambiguate constructors.
   YM_CREATE_TAG_DISPATCH_TYPE(ConstBuffer)

   /**
    * @name StackBuffer_Base Constructors.
    * @{
    * @brief Constructor.
    *
    * - If you try to pass a const buffer wihtout the associated tag it will correctly
    *   fail to compile. You are allowed to pass a non-const buffer as const, but that
    *   would not make sense.
    *
    * @param buffer_Ptr -- Pointer to stack allocated buffer.
    * @param Size       -- Size of buffer.
    */
   explicit constexpr StackBuffer_Base(
      bound<void> const buffer_Ptr,
      std::size_t const Size) noexcept :
         std::pmr::monotonic_buffer_resource(
            buffer_Ptr,
            Size,
            MemIO::getNullMemResource()),
         _Ptr    {buffer_Ptr},
         _Size   {   Size   },
         _nUsers {getNUsers(Qualifier_T::NonConst)}
   { }

   explicit constexpr StackBuffer_Base(
      bound<void const> const buffer_Ptr,
      std::size_t       const Size,
      ConstBuffer       const) noexcept :
         std::pmr::monotonic_buffer_resource(
            MemIO::getNullMemResource()),
         _Ptr    {const_cast<void*>(buffer_Ptr.get())},
         _Size   {            Size             },
         _nUsers {getNUsers(Qualifier_T::Const)}
   { }
   /// @}

   /// @brief Destructor.
   inline virtual ~StackBuffer_Base(void) noexcept = default;

   YM_NO_COPY(StackBuffer_Base)
   YM_NO_ASSIGN(StackBuffer_Base)

   /**
    * @name StackBuffer_Base getters.
    * @{
    * @brief Interprets the data as another type.
    *
    * @tparam T -- Type to interpret the bytes as.
    *
    * @returns loose<T> -- Pointer to data as the desired type.
    */
   template <typename T>
   constexpr loose<T> get(void) noexcept {
      return {std::as_const(*this).get<T>(), ym_PtrCastPassKey{}};
   }

   template <typename T>
   requires (!std::is_array_v<T>)
   constexpr loose<T const> get(void) const noexcept {
      return (_Size >= sizeof(T)) ? std::start_lifetime_as<T>(_Ptr) : nullptr;
   }
   template <typename T>
   requires (std::is_array_v<T>)
   constexpr loose<T const> get(void) const noexcept {
      return (_Size >= sizeof(T)) ? std::start_lifetime_as_array<T>(_Ptr, _Size) : nullptr;
   }
   /// @}

   /**
    * @brief Interprets the data as a byte-like type.
    *
    * @tparam T -- Type to interpret the bytes as.
    *
    * @returns bound<T> -- Pointer to data as the desired type.
    */
   template <typename T = char const>
   requires (sizeof(T) == 1uz)
   constexpr bound<T> getBytes(void) noexcept {
      return {_Ptr, ym_PtrCastPassKey{}};
   }
   template <typename T = char const>
   requires (sizeof(T) == 1uz)
   constexpr bound<T const> getBytes(void) const noexcept {
      return {_Ptr, ym_PtrCastPassKey{}};
   }

private:
   static constexpr inline auto SingleUser_Unclaimed = std::numeric_limits<std::size_t>::max();
   static constexpr inline auto SingleUser_Claimed   = SingleUser_Unclaimed - 1uz;
   static constexpr inline auto MultiUser_Init       = 0uz;

   /// @cond INTERNAL
   constexpr std::size_t getNUsers(Qualifier_T const Q) {
      if (Q == Qualifier_T::Const) {
         return MultiUser_Init;
      } else {
         return SingleUser_Unclaimed;
      }
   }
   /// @endcond

   bound<void> const _Ptr;
   std::size_t const _Size   {        0uz         };
   std::size_t       _nUsers {SingleUser_Unclaimed};
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

   /// @brief Destructor.
   inline virtual ~StackBuffer(void) noexcept = default;

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
      buffer_Ptr->addUser();
   }
};

// ----------------------------------------------------------------------------
//                             Convenience classes
// ----------------------------------------------------------------------------

/**
 * @brief Represents a string literal
 */
class StackStrLit : public StackBuffer_Base
{
public:
   /**
    * @brief Constructor.
    *
    * @tparam N -- Size of array.
    *
    * @param Array -- Pointer to array.
    */
   template <std::size_t N>
   implicit inline StackStrLit(char const (&Array) [N]) noexcept :
      StackBuffer_Base(Array, N, ConstBuffer{})
   { }

   /**
    * @brief Constructor.
    *
    * - This overload should be rarely used as the above will bind to c-style string literals.
    *   We use this to define a user defined literal.
    *
    * @param Array -- Pointer to array.
    * @param N     -- Size of array.
    */
   implicit inline StackStrLit(
      rawstr      const Array,
      std::size_t const N) noexcept :
         StackBuffer_Base(Array, N, ConstBuffer{})
   { }

   /// @brief Destructor.
   inline virtual ~StackStrLit(void) noexcept = default;

   // TODO
   inline str getRaw(void) const noexcept {
      return getBytes();
   }
};

/// @brief TODO
constexpr inline auto operator""_ssl(rawstr const S, std::size_t const N) {
   return StackStrLit(S, N);
}

} // ym
