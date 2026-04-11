/**
 * @file    datalogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "logger.h"
#include "nameable.h"
#include "ymglobals.h"

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
 * @note Implemented as a circular buffer. Stores the last X data entries for each tracked variable.
 *       Each row of the buffer contains all variable values. The names and conversion classes for
 *       these values are stored in a separate array for efficiency.
 *
 * @note *Not* thread-safe.
 */
class DataLogger : public Logger
{
public:
   /** DumpMode_T
    *
    * @brief Mode to indicate how to format the data when writing.
    */
   enum class DumpMode_T
   {
      Text,
      Binary
   };

   /** Options_T
    *
    * @brief Options surrounding opening and writing to a file.
    */
   struct Options_T : public Logger::Options_T
   {
      /// @brief Mode to determine the format to write the data in.
      DumpMode_T _dumpMode{DumpMode_T::Text};

      /// @brief Allows direct comparison between Options_T and specified field type.
      constexpr friend bool operator == (Options_T const & Opts, DumpMode_T const Mode) noexcept {
         return Opts._dumpMode == Mode;
      }
   };

   /// @brief Gets this logger's options.
   inline virtual Options_T const & getOptions(void) const noexcept override { return _Options; }

   explicit DataLogger(
      std::size_t const MaxDepth,
      std::size_t const NTrackedValsHint = 0uz);

   YM_NO_COPY  (DataLogger)
   YM_NO_ASSIGN(DataLogger)

   YM_DECL_YMASSERT(Error)

   bool init(void) noexcept;

   inline auto getMaxDepth  (void) const noexcept { return _MaxDepth;    }
   inline auto isInitialized(void) const noexcept { return _initialized; }

   /// @brief Forwarding function.
   template <typename T>
   inline void track(
         strlit    const Name,
         T const * const Read_Ptr) {
      return track(Name, BoundPtr(Read_Ptr));
   }

   template <typename T>
   void track(
      strlit            const Name,
      BoundPtr<T const> const Read_BPtr); // TODO can be noexcept

   void acquire(void) noexcept;
   void reset(void) noexcept;
   bool dump(strlit const Filename);

protected:
   inline virtual void producer(
      [[maybe_unused]] strlit const     Format,
      [[maybe_unused]] fmt::format_args args) noexcept override {};

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
   class TrackedValBase : public Nameable_NV<strlit>
   {
   public:
      explicit inline TrackedValBase(
         strlit               const Name,
         BoundPtr<void const> const Read_BPtr,
         std::size_t          const Size_bytes);

      virtual ~TrackedValBase(void) = default;

      virtual void cloneAt(
         BoundPtr<void> const val_BPtr,
         std::size_t    const Size_bytes) const = 0;

      virtual void toStr(
         BoundPtr<void const> const Entry_BPtr,
         std::span<char>            buffer) const = 0;

      BoundPtr<void const> const _Read_BPtr;
      std::size_t          const _Size_bytes;

   protected:
      void toStr_Handler(
         std::span<char>  buffer,
         fmt::format_args args) const;
   };

   /** TrackedVal
    *
    * @brief Stores a reference to a variable and provides stringification.
    */
   template <typename T>
   class TrackedVal : public TrackedValBase
   {
   public:
      explicit inline TrackedVal(
         strlit               const Name,
         BoundPtr<void const> const Read_BPtr);

      virtual void cloneAt(
         BoundPtr<void> const val_BPtr,
         std::size_t    const Size_bytes) const override;

      virtual void toStr(
         BoundPtr<void const> const Entry_BPtr,
         std::span<char>            buffer) const override;
   };

   using RawTrackedVal_T = PolyRaw<TrackedValBase, sizeof(TrackedVal<int>)>;

   std::pmr::vector<
      RawTrackedVal_T>     _trackedVals   {    };
   std::pmr::vector<uchar> _blackBoxBuffer{    };
   std::size_t const       _MaxDepth      {10uz};
   Options_T const _Options  {  /* default */  };
   union {
      std::size_t          _nTrackedValsHint{0uz};
      std::size_t          _nextEntry_idx;
   };
   bool                    _rollover   {false};
   bool                    _initialized{false};
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
   strlit            const Name,
   BoundPtr<T const> const Read_BPtr)
{
   _trackedVals.emplace_back(std::in_place_type<TrackedVal<T>>, Name, Read_BPtr);
}

/** TrackedValBase
 *
 * @brief Constructor.
 */
inline DataLogger::TrackedValBase::TrackedValBase(
   strlit               const Name,
   BoundPtr<void const> const Read_BPtr,
   std::size_t          const Size_bytes) :
      Nameable_NV(Name),
      _Read_BPtr  {Read_BPtr },
      _Size_bytes {Size_bytes}
{ }

/** TrackedVal
 *
 * @brief Constructor.
 */
template <typename T>
DataLogger::TrackedVal<T>::TrackedVal(
   strlit               const Name,
   BoundPtr<void const> const Read_BPtr) :
      TrackedValBase(Name, Read_BPtr, sizeof(T))
{ }

/** cloneAt
 *
 * @brief Clones class at specified location (copy idiom for polymorphic types).
 *
 * @param val_BPtr   -- Pointer to variable to bbe tracked.
 * @param Size_bytes -- Size of variable in bytes.
 */
template <typename T>
void DataLogger::TrackedVal<T>::cloneAt(
   BoundPtr<void> const val_BPtr,
   std::size_t    const Size_bytes) const
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
 * @tparam T -- Type of variable to convert to.
 *
 * @param DataEntry_Ptr -- Pointer to data to stringify.
 */
template <typename T>
void DataLogger::TrackedVal<T>::toStr(
   BoundPtr<void const> const Entry_BPtr,
   std::span<char>            buffer) const
{
   toStr_Handler(buffer, fmt::make_format_args(*static_cast<T const *>(Entry_BPtr.get())));
}

} // ym
