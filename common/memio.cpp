/**
 * @file    argparser.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "memio.h"

/** getNullMemResource
 *
 * @brief Returns a static pointer to a custom memory resource.
 *
 * @returns BoundPtr<std::pmr::memory_resource> -- Pointer to custom null memory resource.
 */
auto ym::MemIO::getNullMemResource(void) noexcept -> BoundPtr<std::pmr::memory_resource>
{
   class NullMemResource final : public std::pmr::memory_resource
   {
      virtual void * do_allocate(
         [[maybe_unused]] std::size_t const Bytes,
         [[maybe_unused]] std::size_t const Alignment) override
      {
         YMASSERT(false, ym_MemResourceError, YM_DAH, "Null memory resource");
         return nullptr;
      }

      virtual void do_deallocate(
         [[maybe_unused]] void *      const Ptr,
         [[maybe_unused]] std::size_t const Bytes,
         [[maybe_unused]] std::size_t const Alignment) noexcept override
      { }

      virtual bool do_is_equal(const std::pmr::memory_resource & other_ref) const noexcept override
      {
         return this == &other_ref;
      }
   };

   static NullMemResource resource;
   return {&resource, ym_AssumePtrNotNull{}};
}
