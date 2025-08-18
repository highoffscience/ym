/**
 * @file    datalogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include "logger.h"
#include "memio.h"
#include "nameable.h"

#include "fmt/base.h"

#include <memory_resource>
#include <vector>

namespace ym
{

/** DataLogger
 *
 * @brief A blackbox.
 * 
 * @note Implemented as a circular buffer. Stores the last X data entries.
 * 
 * @note *Not* thread-safe.
 */
class DataLogger : public Logger
{
public:
   explicit DataLogger(sizet const HintNEntries = 0uz); // TODO rename (Data)Entries variable to something else
   // (Data)Entries (used below) should be called (Data)Points.

   YM_NO_COPY  (DataLogger)
   YM_NO_ASSIGN(DataLogger)

   YM_DECL_YMASSERT(Error)

   bool ready(
      sizet                           const MaxNEntries,
      bptr<std::pmr::memory_resource> const MemSrc = std::pmr::get_default_resource());

   inline auto getMaxNDataEntries(void) const { return _blackBoxBuffer.size() / _columnHeaders.size(); }

   /// @brief Forwarding function.
   template <typename T>
   inline void addEntry(
      str const Name,
      T * const read_Ptr) { return addEntry(Name, bptr(read_Ptr)); }

   template <typename T>
   void addEntry(
      str     const Name,
      bptr<T> const Read_BPtr);

   void acquireAll(void);
   void reset(void);
   bool dump(str const Filename);

private:
   /** ColumnHeaderBase
    * 
    * @brief Meta data carrier.
    * 
    * @note We store a raw pointer to the data we wish to capture. Optionally using a lambda
    *       to capture data requires too slow of a mechanism. Data loggers should be as
    *       non-invasive as possible to prevent interfering with the actual intention of the
    *       program. This alone isn't actually a problem because debugging logic can be
    *       thought of as part of the execution, not just as an observer interfering with
    *       it. However, we still want as much attention on the intention of the program,
    *       not debugging, so how the data logger acquires it's information should be as
    *       efficient as possible.
    * 
    * @note The pointer stored for the variable is non-owning. Per efficiency concerns
    *       discussed above we use raw pointers instead of shared pointers. Also shared
    *       pointers don't really make sense because they might point to things on the
    *       stack. Care must be taken the data logger doesn't outlive the tracked variables.
    * 
    * @note This class does *not* own the read pointer.
    */
   class ColumnHeaderBase : public Nameable_NV<str>
   {
   public:
      explicit inline ColumnHeaderBase(
         str              const Name,
         bptr<void const> const Read_BPtr,
         sizet            const Size_bytes);

      virtual ~ColumnHeaderBase(void) = default;

      virtual void toStr(
         bptr<void const> const Entry_BPtr,
         bptr<char>       const buffer_BPtr) const = 0;

      bptr<void const> const _Read_BPtr;
      sizet            const _Size_bytes;

   protected:
      void toStr_Handler(
         bptr<char> const buffer_BPtr,
         fmt::format_args args) const;
   };

   /**
    * TODO
    */
   template <typename T>
   class ColumnHeader : public ColumnHeaderBase
   {
   public:
      explicit inline ColumnHeader(
         str           const Name,
         bptr<T const> const Read_BPtr);

      virtual void toStr(
         bptr<void const> const Entry_BPtr,
         bptr<char>       const buffer_BPtr) const override;
   };

   static_assert(sizeof(ColumnHeaderBase) == sizeof(ColumnHeader<int>), "Potential slicing hazard");

   std::pmr::vector<
      ColumnHeaderBase>    _columnHeaders {   };
   std::pmr::vector<uint8> _blackBoxBuffer{   };
   sizet                   _nextEntry_idx {0uz};
};

/** addEntry
 * 
 * @brief Adds a data variable to be tracked.
 * 
 * @tparam T -- Data type to add.
 * 
 * @param Name      -- Name of variable.
 * @param Read_BPtr -- Pointer to variable to be read.
 */
template <typename T>
void DataLogger::addEntry(
   [[maybe_unused]] str     const Name,
   [[maybe_unused]] bptr<T> const Read_BPtr)
{
   // not alloc<T> because the deallocator needs the type and we lose the type after this call
   // The above statement could be solved by moving the destructor into Stringify.
   // auto const dataEntries_BPtr = bptr(MemIO::alloc<uint8>(getMaxNDataEntries() * sizeof(T)));
   // auto ce = ColumnEntry(Name, dataEntries_BPtr, Read_BPtr.get(), new Stringify<T>());
   // _columnEntries.emplace_back(ce);
   // _columnEntries.emplace_back(Name, draw, Read_BPtr.get(), new Stringify<T>());
}

/** ColumnHeaderBase
 * 
 * @brief Constructor.
 */
inline DataLogger::ColumnHeaderBase::ColumnHeaderBase(
   str              const Name,
   bptr<void const> const Read_BPtr,
   sizet            const Size_bytes) :
      Nameable_NV(Name),
      _Read_BPtr  {Read_BPtr },
      _Size_bytes {Size_bytes}
{ }

/**
 * TODO
 * 
 */
template <typename T>
DataLogger::ColumnHeader<T>::ColumnHeader(
   str           const Name,
   bptr<T const> const Read_BPtr) :
      ColumnHeaderBase(Name, Read_BPtr, sizeof(T))
{ }

/** toStr
 * 
 * @brief Stringifies the given data type.
 * 
 * @note To accomodate types not supported by std::to_string() you can either
 *       specialize Stringify or specialize std::to_string().
 * 
 * @tparam T -- Type of variable to convert to.
 * 
 * @param DataEntry_Ptr -- Pointer to data to stringify.
 */
template <typename T>
void DataLogger::ColumnHeader<T>::toStr(
   bptr<void const> const Entry_BPtr,
   bptr<char>       const buffer_BPtr) const
{
   toStr_Handler(buffer_BPtr, fmt::make_format_args(*static_cast<T const *>(Entry_BPtr.get())));
}

} // ym
