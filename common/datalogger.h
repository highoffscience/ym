/**
 * @file    datalogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include "logger.h"
#include "nameable.h"

#include "fmt/base.h"

#include <memory_resource>
#include <span>
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
   explicit DataLogger(
      sizet const MaxNEntries,
      sizet const NTrackedValsHint = 0uz);

   YM_NO_COPY  (DataLogger)
   YM_NO_ASSIGN(DataLogger)

   YM_DECL_YMASSERT(Error)

   bool ready(void);

   inline auto getMaxNDataEntries(void) const { return _MaxNDataEntries; }
   inline auto isInitialized     (void) const { return _initialized;     }

   /// @brief Forwarding function.
   template <typename T>
   inline void track(
      str       const Name,
      T const * const Read_Ptr) { return track(Name, bptr(Read_Ptr)); }

   template <typename T>
   void track(
      str           const Name,
      bptr<T const> const Read_BPtr);

   void acquireAll(void);
   void reset(void);
   bool dump(
      str              const   Filename,
      OpeningOptions_T const & Options = {});

private:
   /** TrackedValBase
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
   class TrackedValBase : public Nameable_NV<str>
   {
   public:
      explicit inline TrackedValBase(
         str              const Name,
         bptr<void const> const Read_BPtr,
         sizet            const Size_bytes);

      virtual ~TrackedValBase(void) = default;

      virtual void cloneAt(
         bptr<void> const val_BPtr,
         sizet      const Size_bytes) const = 0;

      virtual void toStr(
         bptr<void const> const Entry_BPtr,
         std::span<char>        buffer) const = 0;

      bptr<void const> const _Read_BPtr;
      sizet            const _Size_bytes;

   protected:
      void toStr_Handler(
         std::span<char>  buffer,
         fmt::format_args args) const;
   };

   /**
    * TODO
    */
   template <typename T>
   class TrackedVal : public TrackedValBase
   {
   public:
      explicit inline TrackedVal(
         str              const Name,
         bptr<void const> const Read_BPtr);

      virtual void cloneAt(
         bptr<void> const val_BPtr,
         sizet      const Size_bytes) const override;

      virtual void toStr(
         bptr<void const> const Entry_BPtr,
         std::span<char>        buffer) const override;
   };

   using RawTrackedVal_T = PolyRaw<TrackedValBase, sizeof(TrackedVal<int>)>;

   std::pmr::vector<
      RawTrackedVal_T>    _trackedVals   {};
   std::pmr::vector<byte> _blackBoxBuffer{};
   union {
      sizet               _nTrackedValsHint{0uz};
      sizet               _nextEntry_idx;
   };
   sizet const            _MaxNDataEntries{10uz };
   bool                   _rollover       {false};
   bool                   _initialized    {false};
};

/** track
 * 
 * @brief Adds a data variable to be tracked.
 * 
 * @tparam T -- Data type to add.
 * 
 * @param Name      -- Name of variable.
 * @param Read_BPtr -- Pointer to variable to be read.
 */
template <typename T>
void DataLogger::track(
   str           const Name,
   bptr<T const> const Read_BPtr)
{
   _trackedVals.emplace_back(); // push allocated memory
   _trackedVals.back().construct<TrackedVal<T>>(Name, Read_BPtr);
}

/** TrackedValBase
 * 
 * @brief Constructor.
 */
inline DataLogger::TrackedValBase::TrackedValBase(
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
DataLogger::TrackedVal<T>::TrackedVal(
   str              const Name,
   bptr<void const> const Read_BPtr) :
      TrackedValBase(Name, Read_BPtr, sizeof(T))
{ }

/**
 * TODO
 * 
 */
template <typename T>
void DataLogger::TrackedVal<T>::cloneAt(
   bptr<void> const val_BPtr,
   sizet      const Size_bytes) const
{
   YMASSERT(sizeof(*this) <= Size_bytes, Error, YM_DAH,
      "Not enough room to clone (obj {} bytes, buffer {} bytes)",
      sizeof(*this), Size_bytes)
   
   ::new (val_BPtr.get()) TrackedVal<T>(getName(), _Read_BPtr);
}

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
void DataLogger::TrackedVal<T>::toStr(
   bptr<void const> const Entry_BPtr,
   std::span<char>        buffer) const
{
   toStr_Handler(buffer, fmt::make_format_args(*static_cast<T const *>(Entry_BPtr.get())));
}

} // ym
