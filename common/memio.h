/**
 * @file    memio.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include <alloca.h>
#include <cstdint>
#include <exception>
#include <forward_list>
#include <memory>
#include <type_traits>

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
 * @ref <https://man7.org/linux/man-pages/man3/alloca.3.html>.
 * @ref <https://en.cppreference.com/w/c/language/array>. See section on VLA's.
 *
 * @param Type_      -- Type to allocate.
 * @param NElements_ -- Number of T elements to allocate room for.
 *
 * @returns Type_ * -- Pointer to newly allocated stack memory.
 */
#define YM_STACK_ALLOC(Type_, NElements_) \
   static_cast<Type_ *>(alloca((NElements_) * sizeof(Type_)))

/** MemIO
 *
 * @brief Class that provides memory management utilities.
 */
class MemIO
{
public:
   YM_NO_DEFAULT(MemIO)

   YM_DECL_YMASSERT(Error)

   template <typename T>
   static T * alloc(uint64 const NElements = 1_u64);

   template <typename T>
   static void dealloc(
      T *    const data_Ptr,
      uint64 const NElements); // no default intentional

   template <typename T>
   static auto alloc_smart(uint64 const NElements); // return type not known yet
};

/** alloc
 * 
 * @brief Allocates raw chunk of memory.
 * 
 * @note Take a couple teaspoons of cement and harden the hell up.
 * 
 * @tparam T -- Type of data to allocate.
 * 
 * @param NElements -- Number of elements to allocate.
 * 
 * @returns T * -- Pointer to allocated memory.
 */
template <typename T>
T * MemIO::alloc(uint64 const NElements)
{
   T * buffer_ptr = nullptr;

   YMASSERT(NElements > 0_u64, Error, YM_DAH, "Cannot alloc 0 elements")

   try
   { // alloc can fail
      std::allocator<T> a;
      buffer_ptr = a.allocate(NElements);
   }
   catch (std::exception const & E)
   { // failed to alloc
      YMASSERT(NElements > 0_u64, Error, YM_DAH, "a.allocate() failed")
   }

   return buffer_ptr;
}

/** dealloc
 * 
 * @brief Deallocates chunk of memory previously allocated by alloc().
 * 
 * @note NElements must be the same as used in the call to alloc().
 * 
 * @tparam T -- Type of data to deallocate.
 * 
 * @param NElements -- Number of elements to deallocate.
 */
template <typename T>
void MemIO::dealloc(
   T *    const data_Ptr,
   uint64 const NElements)
{
   std::allocator<T> a;
   a.deallocate(data_Ptr, NElements);
}

/** alloc_smart
 * 
 * @brief Allocates raw chunk of memory wrapped in a smart pointer.
 * 
 * @tparam T -- Type of data to allocate.
 * 
 * @param NElements -- Number of elements to allocate.
 * 
 * @returns std::unique_ptr -- Smart pointer to allocated memory.
 */
template <typename T>
auto MemIO::alloc_smart(uint64 const NElements) // -> std::unique_ptr
{
   auto deleter = [NElements](T * const datum_Ptr) {
      if (datum_Ptr) {
         dealloc<T>(datum_Ptr, NElements);
      }
   };

   return std::unique_ptr<T, decltype(deleter)>(alloc<T>(NElements), deleter);
}

} // ym
