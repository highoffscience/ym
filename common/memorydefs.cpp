/**
 * @author Forrest Jablonski
 */

#include "memorydefs.h"

std::unordered_map<void *, ym::uint64> ym::MemoryDefs::_allocatedSizes; // default

/**
 *
 */
void ym::MemoryDefs::deallocate(void * const data_Ptr)
{
   auto const It = _allocatedSizes.find(data_Ptr);

   ymAssert(It != _allocatedSizes.end(),
      "Trying to deallocate storage at %p where none was allocated!",
      data_Ptr);

   std::allocator<uint8> a;
   a.deallocate(static_cast<uint8 *>(data_Ptr), It->second);

   _allocatedSizes.erase(It);
}

/**
 * TODO this function needs work
 */
void * ym::MemoryDefs::allocate(uint64 const NElements,
                                uint32 const SizeOfDataType)
{
   auto const NBytes = NElements * SizeOfDataType;

   auto const Alignment = (SizeOfDataType <= 16u) ? std::align_val_t{16u} :
                          (SizeOfDataType <= 32u) ? std::align_val_t{32u} :
                                                    std::align_val_t{64u};
                                                    
   auto * const data_Ptr = ::operator new(NBytes, Alignment);

   ymAssert(data_Ptr, "Alignment new failed!");

   _allocatedPtrs.emplace_back(data_Ptr, NBytes);

   return data_Ptr;
}
