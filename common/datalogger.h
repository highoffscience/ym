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
   explicit DataLogger(uint64 const MaxNDataEntries);
   ~DataLogger(void);

   YM_NO_COPY  (DataLogger)
   YM_NO_ASSIGN(DataLogger)

   YM_DECL_YMASSERT(Error)

   inline auto getMaxNDataEntries(void) const { return _MaxNDataEntries; }

   template <typename T>
   void addEntry(
      str     const Name,
      bptr<T> const Read_BPtr);

   void acquireAll(void);
   void clear(void);
   bool dump(str const Filename);

private:
   /** IStringify
    * 
    * @brief Base class for stringying arbitrary data types.
    */
   struct IStringify
   {
      virtual ~IStringify(void) = default;
      virtual void toStr(
         void const * const DataEntry_Ptr,
         char       * const buffer_Ptr) const = 0;

   private:
      void toStr_Handler(
         char * const     buffer_Ptr,
         fmt::format_args args) const;
   };

   /** Stringify
    * 
    * @brief Concrete class for stringying arbitrary data types.
    */
   template <typename T>
   struct Stringify : public IStringify
   {
      virtual void toStr(
         void const * const DataEntry_Ptr,
         char       * const buffer_Ptr) const override;
   };

   /** ColumnEntry
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
    * @note This class does *not* own any of the pointers it has.
    */
   struct ColumnEntry : public Nameable_NV<str>
   {
      template <typename T>
      explicit inline ColumnEntry(
         str                const Name,
         uint8            * const dataEntries_Ptr,
         T          const * const Read_Ptr,
         IStringify const * const Stringify_Ptr);

      uint8            * const _dataEntries_Ptr{};
      void       const * const _Read_Ptr       {};
      IStringify const * const _Stringify_Ptr  {};
      uint64             const _DataSize_bytes {};
   };

   std::vector<ColumnEntry> _columnEntries    {};
   uint64 const             _MaxNDataEntries  {};
   uint64                   _nextDataEntry_idx{};
   bool                     _rollover         {};
};

/** addEntry
 * 
 * @brief Adds a data variable to be tracked.
 * 
 * @tparam T -- Data type to add.
 * 
 * @param Name     -- Name of variable.
 * @param Read_Ptr -- Pointer to variable to be read.
 */
template <typename T>
void DataLogger::addEntry(
   str     const Name,
   bptr<T> const Read_BPtr)
{
   // not alloc<T> because the deallocator needs the type and we lose the type after this call
   auto const dataEntries_BPtr = bptr(MemIO::alloc<uint8>(getMaxNDataEntries() * sizeof(T)));
   _columnEntries.emplace_back(Name, dataEntries_BPtr, Read_BPtr, new Stringify<T>());
}

/** Stringify
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
void DataLogger::Stringify<T>::toStr(
   void const * const DataEntry_Ptr,
   char       * const buffer_Ptr) const
{
   toStr_Handler(buffer_Ptr, fmt::make_format_args(*static_cast<T const *>(DataEntry_Ptr)));
}

/** ColumnEntry
 * 
 * @brief Constructor.
 */
template <typename T>
inline DataLogger::ColumnEntry::ColumnEntry(
   str                const Name,
   uint8            * const dataEntries_Ptr,
   T          const * const Read_Ptr,
   IStringify const * const Stringify_Ptr) :
      Nameable_NV(Name),
      _dataEntries_Ptr {dataEntries_Ptr},
      _Read_Ptr        {Read_Ptr       },
      _Stringify_Ptr   {Stringify_Ptr  },
      _DataSize_bytes  {sizeof(T)      }
{ }

} // ym
