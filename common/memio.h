/**
 * @file    memio.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "textlogger.h"
#include "ymception.h"

#include <alloca.h>
#include <cstdint>
#include <exception>
#include <forward_list>
#include <memory>

/* TODO custom stack based allocator for stl types

#include <iostream>
#include <string>

char g_buf[100] = {'\0'};
char * g_p = g_buf;

template <typename T>
struct CustomAlloc
{
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    
    constexpr CustomAlloc(void) = default;
    constexpr ~CustomAlloc(void) = default;
    
    constexpr T * allocate(std::size_t n)
    {
        std::cout << "allocate " << n << std::endl;
        auto * r = g_p;
        g_p += n;
        return r;
    }
    
    constexpr void deallocate(T * p, std::size_t n)
    {
        (void)p;
        std::cout << "deallocate " << n << std::endl;
    }
    
    constexpr std::size_t max_size(void) const
    {
        std::cout << "max_size" << std::endl;
        return 0u;
    }
};

int main()
{
    using custom_string = std::basic_string<char, std::char_traits<char>, CustomAlloc<char>>;

    custom_string s("go! torchic!"); // small string optimization
    custom_string s2("go! torchic! gooooo!");
    
    std::cout << s2.c_str() << std::endl;
    std::cout << g_buf << std::endl;
    std::cout << (void*)s2.data() << std::endl;
    std::cout << (void*)g_buf << std::endl;
    std::cout << (void*)g_p << std::endl;

    return 0;
}
*/

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

/** Chunkable_T
 *
 * @brief Represents one datum element in a memory pool.
 *
 * @tparam T -- Type that is chunkable.
 */
template <typename T>
concept Chunkable_T = (sizeof(T) >= sizeof(uintptr));

/** MemIO
 *
 * @brief Class that provides memory management utilities.
 */
class MemIO
{
public:
   YM_NO_DEFAULT(MemIO)

   YM_DECL_YMCEPT(MemIOError)
   YM_DECL_YMCEPT(MemIOError, MemIOError_PoolError)

   template <typename T>
   static T * alloc(uint64 const NElements = 1_u64);

   template <typename T>
   static void dealloc(T *    const data_Ptr,
                       uint64 const NElements); // no default intentional

   template <typename T>
   static auto alloc_smart(uint64 const NElements); // return type not known yet

   /** Pool
    *
    * @brief Class that manages a particular pool of memory.
    */
   template <Chunkable_T Chunk_T>
   class Pool
   {
      friend class MemIO; // access to constructor

   public:
      ~Pool(void);

      Chunk_T * alloc      (void);
      auto      alloc_smart(void); // return type not known yet

      void dealloc(Chunk_T * const datum_Ptr);

      constexpr auto getNChunksPerBlock(void) const { return _NChunksPerBlock; }

   private:
      explicit Pool(uint64 const NChunksPerBlock);

      std::forward_list<Chunk_T *> _blocks;
      Chunk_T *                    _nextFreeChunk_ptr;
      uint64 const                 _NChunksPerBlock;
   };

   template <Chunkable_T Chunk_T>
   static Pool<Chunk_T> getNewPool(uint64 nChunksPerBlock = 256_u64);

   /** Pool
    *
    * @brief Non-null pointer.
    * 
    * @tparam T -- Type of pointer.
    * 
    * @note Throwing in the constructor is preferable because you cannot swallow the
    *       exception and use BoundedPtr in an unacceptable state.
    */
   template <typename T>
   class BoundedPtr
   {
   public:
      YM_IMPLICIT constexpr BoundedPtr(T * const t_Ptr)
         : _t_ptr {t_Ptr}
      {
         // TODO assert non null
      }

      constexpr BoundedPtr              (std::nullptr_t) = delete;
      constexpr BoundedPtr & operator = (std::nullptr_t) = delete;

      constexpr BoundedPtr & operator = (T * const t_Ptr)
      {
         // TODO assert non null
         _t_ptr = t_Ptr;
         return *this;
      }

      constexpr T       * get        (void)       { return _t_ptr; }
      constexpr T const * get        (void) const { return _t_ptr; }

      constexpr T       & operator * (void)       { return *get(); }
      constexpr T const & operator * (void) const { return *get(); }

      constexpr T       * operator -> (void)       { return get(); }
      constexpr T const * operator -> (void) const { return get(); }

   private:
      T * _t_ptr;
   };
};

// TODO create ymcommon.h to accomodate classes that need ymception.h, etc.
// template <typename T>
// using bptr = BoundedPtr<T>;
//
// using str = bptr<char const>;
//
// TODO create user-defined literal for str

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

   try
   { // alloc can fail
      MemIOError::check(NElements > 0_u64, "Cannot alloc 0 elements");

      std::allocator<T> a;
      buffer_ptr = a.allocate(NElements);
   }
   catch (std::exception const & E)
   { // failed to alloc
      ymLog(VG::Error, "Error in MemIO::alloc(): %s", E.what());
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
void MemIO::dealloc(T *    const data_Ptr,
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

/** getNewPool
 *
 * @brief Creates and returns a new memory pool.
 *
 * @tparam Chunk_T -- Type the pool contains.
 * 
 * @param nChunksPerBlock -- Number of chunks (datum elements) per block of memory.
 *
 * @returns Pool<Chunk_T> -- Pool object.
 */
template <Chunkable_T Chunk_T>
auto MemIO::getNewPool(uint64 nChunksPerBlock) -> Pool<Chunk_T>
{
   if (nChunksPerBlock == 0_u64)
   {
      constexpr auto DefaultNChunksPerBlock = 256_u64;
      ymLog(VG::Error, "Error in MemIO::getNewPool(): # of chunks per block must be > 0.",
         " Defaulting to %lu", DefaultNChunksPerBlock);
      nChunksPerBlock = DefaultNChunksPerBlock;
   }

   return Pool<Chunk_T>(nChunksPerBlock);
}

/** Pool
 *
 * @brief Constructor.
 *
 * @note NChunksPerBlock already checked to be > 0.
 * 
 * @tparam Chunk_T -- Type the pool contains.
 *
 * @param NChunksPerBlock -- Chunks (datum elements) per block of memory.
 */
template <Chunkable_T Chunk_T>
MemIO::Pool<Chunk_T>::Pool(uint64 const NChunksPerBlock)
   : _blocks            {nullptr        },
     _nextFreeChunk_ptr {nullptr        },
     _NChunksPerBlock   {NChunksPerBlock}
{
}

/** ~Pool
 *
 * @brief Destructor.
 *
 * @tparam Chunk_T -- Type the pool contains.
 */
template <Chunkable_T Chunk_T>
MemIO::Pool<Chunk_T>::~Pool(void)
{
   for (auto * const block_Ptr : _blocks)
   {
      MemIO::dealloc<Chunk_T>(block_Ptr, getNChunksPerBlock());
   }
}

/** alloc
 *
 * @brief Returns a pointer to a block of raw memory equal to sizeof(Chunk_T).
 *
 * @tparam Chunk_T -- Type the pool contains.
 * 
 * @returns Pointer to block of raw memory equal to sizeof(Chunk_T).
 */
template <Chunkable_T Chunk_T>
Chunk_T * MemIO::Pool<Chunk_T>::alloc(void)
{
   if (!_nextFreeChunk_ptr)
   { // current pool is exhausted - create another one

      // TODO MemIO::alloc might return null - check for this case!
      _blocks.emplace_front(MemIO::alloc<Chunk_T>(getNChunksPerBlock()));
      _nextFreeChunk_ptr = _blocks.front();

      PtrToInt_T<Chunk_T> curr{_nextFreeChunk_ptr};
      for (auto i = 0_u64; i < getNChunksPerBlock(); ++i)
      { // set "next" pointer for each chunk
         *curr.uint_ptr_val = curr.uint_val + sizeof(Chunk_T);
         curr.uint_val      = *curr.uint_ptr_val;
      }
      curr.ptr_val -= 1_u64; // this is valid because getNChunksPerBlock() is > 0
      *curr.uint_ptr_val = 0_u64; // init last next ptr to null
   }
   
   union
   {
      Chunk_T *   ptr_val;
      Chunk_T * * ptr_ptr_val;
   } datum{_nextFreeChunk_ptr};

   _nextFreeChunk_ptr = *datum.ptr_ptr_val;

   return datum.ptr_val;
}

/** alloc_smart
 *
 * @brief Returns a smart pointer to a block of raw memory equal to sizeof(Chunk_T).
 * 
 * @note Do unto others before they do unto me.
 * 
 * @tparam Chunk_T -- Type the pool contains.
 * 
 * @returns Smart pointer to block of raw memory equal to sizeof(Chunk_T).
 */
template <Chunkable_T Chunk_T>
auto MemIO::Pool<Chunk_T>::alloc_smart(void) // -> std::unique_ptr
{
   auto deleter = [this](Chunk_T * const datum_Ptr) {
      this->dealloc(datum_Ptr);
   };

   return std::unique_ptr<Chunk_T, decltype(deleter)>(alloc(), deleter);
}

/** dealloc
 *
 * @brief Frees chunk of memory to the pool.
 * 
 * @tparam Chunk_T -- Type the pool contains.
 * 
 * @param datum_Ptr -- Chunk of memory to free.
 */
template <Chunkable_T Chunk_T>
void MemIO::Pool<Chunk_T>::dealloc(Chunk_T * const datum_Ptr)
{
   *reinterpret_cast<Chunk_T * *>(datum_Ptr) = _nextFreeChunk_ptr;
   _nextFreeChunk_ptr = datum_Ptr;
}

} // ym
