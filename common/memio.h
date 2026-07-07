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
 * @link <https://man7.org/linux/man-pages/man3/alloca.3.html>.
 * @link <https://en.cppreference.com/w/c/language/array>. See section on VLA's.
 *
 * @param Type_      -- Type to allocate.
 * @param NElements_ -- Number of T elements to allocate room for.
 *
 * @returns Type_ * -- Pointer to newly allocated stack memory.
 */
#define YM_STACK_ALLOC(Type_, NElements_) \
   static_cast<Type_ *>(alloca((NElements_) * sizeof(Type_)))

/// @brief Global memory resource error.
YM_DECL_YMASSERT(ym_MemResourceError)

/**
 * @brief TODO
 *
 */
inline BoundPtr<std::pmr::memory_resource> ymGetNullMemResource(void) noexcept
{
   class type final : public std::pmr::memory_resource
   {
      virtual void * do_allocate(std::size_t, std::size_t) override {
         YMASSERT(false, ym_MemResourceError, YM_DAH, "Null memory resource");
         return nullptr;
      }

      virtual void do_deallocate(void *, std::size_t, std::size_t) noexcept override
      { }

      virtual bool do_is_equal(const std::pmr::memory_resource& __other) const noexcept override {
         return this == &__other;
      }
   };

   alignas(type) static unsigned char __buf[sizeof(type)];
   static type * __r = new(__buf) type;
   return BoundPtr(__r, ym_AssumePtrNotNull{});
}

} // ym
