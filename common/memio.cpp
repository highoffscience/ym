/**
 * @file    argparser.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "memio.h"

/**
 * @brief Returns a static pointer to a custom memory resource.
 *
 * @returns bound<std::pmr::memory_resource> -- Pointer to custom null memory resource.
 */
auto ym::MemIO::getNullMemResource(void) noexcept -> bound<std::pmr::memory_resource>
{
   class NullMemResource final : public std::pmr::memory_resource
   {
      virtual void * do_allocate(
         [[maybe_unused]] std::size_t const Bytes,
         [[maybe_unused]] std::size_t const Alignment) override
      {
         YMASSERT(false, ym_MemResourceError, YM_DAH_RV(nullptr), "Null memory resource");
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

// ----------------------------------------------------------------------------

/**
 * @brief Marks this buffer as claimed.
 *
 * @returns bool -- True if successfully claimed, false if there is already an assigned user.
 */
bool ym::StackBuffer_Base::addUser(void) noexcept
{
   auto success = true; // until told otherwise

   switch (_nUsers)
   { // figure out which mode we are in
      case SingleUser_Unclaimed:
      { // mark buffer as claimed
         _nUsers = SingleUser_Claimed;
         break;
      }
      case SingleUser_Claimed:
      { // buffer already claimed - uh oh
         success = false;
         break;
      }
      default:
      { // increment user count
         _nUsers++;
         break;
      }
   }

   return success;
}
