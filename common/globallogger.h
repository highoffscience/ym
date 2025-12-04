/**
 * @file    globallogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "textlogger.h"
#include "verbogroup.h"
#include "ymglobals.h"

#include "fmt/base.h"

#include <array>
#include <atomic>
#include <bitset>
#include <concepts>
#include <utility>

namespace ym
{

/*
 * Convenience functions.
 * -------------------------------------------------------------------------- */

template <typename... Args_T>
inline void ymLog(
   VF     const VFlag,
   strlit const Format,
   Args_T &&... args_uref);

template <std::same_as<VF>... VFs_T> inline void ymLogEnable (VFs_T const... VFlags);
template <std::same_as<VF>... VFs_T> inline void ymLogDisable(VFs_T const... VFlags);

// template <std::same_as<VF>... VFlags_T>
// inline class ScopedEnable ymLogPushEnable(VFs_T const... VFlags);
// Above is implemented below - ScopedEnable isn't yet defined.

/* -------------------------------------------------------------------------- */

/** GlobalLogger
 *
 * @brief Logs text to the given outfile - similary to std::printf.
 */
class GlobalLogger : public TextLogger
{
public:
   /// @brief Options surrounding file configs.
   using Options_T = TextLogger::Options_T;

   static constexpr Options_T getDefaultOptions(void) { return {}; }
   inline virtual Options_T const & getOptions(void) const override { return _Options; }

   virtual ~GlobalLogger(void);

   YM_NO_COPY  (GlobalLogger)
   YM_NO_ASSIGN(GlobalLogger)

   YM_DECL_YMASSERT(PrintError)
   YM_DECL_YMASSERT(GlobalError)

   static BoundPtr<GlobalLogger> getGlobalInstance(void);

   /** ScopedEnable
    * 
    * @brief Allows managed temporary enabling of a verbosity group.
    * 
    * TODO
    * 
    * @note Uses RAII to storing/restoring enabling verbosity groups.
    * 
    * @note The return value from pushEnable will need to be explicitly stored, ie.
    *       auto const SE = ymLogPushEnable(VF);
    *       even if SE is not used, since the destructor has side effects. Simply calling
    *       pushEnable will result in the ScopedEnable structure being deleted immediately.
    */
   template <std::same_as<VF>... VFs_T>
   class ScopedEnable
   {
   public:
      static constexpr auto NFlags = sizeof...(VFs_T);

      explicit ScopedEnable(VFs_T const... VFlags);
      ~ScopedEnable(void);

      void popEnable(void) const;

   private:
      std::array<VF, NFlags> const _VFlags;
      std::bitset<   NFlags> const _WasEnabled;
   };

   template <std::same_as<VF>... VFs_T> void enable (VFs_T const... VFlags);
   template <std::same_as<VF>... VFs_T> void disable(VFs_T const... VFlags);

   template <std::same_as<VF>... VFs_T>
   ScopedEnable<VFs_T...> pushEnable(VFs_T const... VFlags);

private:
   explicit GlobalLogger(
      strlit    const   Filename,
      Options_T const & Options = getDefaultOptions());

   virtual void producer(
      strlit const     Format,
      fmt::format_args args) override;

   /** State_T
    *
    * @brief State of the logger.
    */
   enum class State_T
   {
      Closed,
      Closing,
      Open,
      Opening
   };
   
   static constexpr auto SlotSize_bytes   = 256uz;
   static constexpr auto SeqNSize_bytes   = sizeof(std::atomic_unsigned_lock_free);
   static constexpr auto MaxMsgSize_bytes = SlotSize_bytes - SeqNSize_bytes;

   struct Slot
   {
      std::atomic_unsigned_lock_free     _seqN     {0u};
      std::array<char, MaxMsgSize_bytes> _msgBuffer{  };
   };

   static_assert(MaxMsgSize_bytes >= 64uz, "Too limited room"); // time stamps require some space
   static_assert(sizeof(Slot) == SlotSize_bytes, "Slot packing not as expected");

   std::array<Slot, 32uz> _slots{};
   std::atomic_unsigned_lock_free _writePos{0u};
   std::atomic_unsigned_lock_free _readPos {0u};

   Options_T const      _Options  { /* default */  };
   VerboGroup           _vGroups  { /* default */  };
   std::atomic<State_T> _state    {State_T::Closed };
   std::atomic_flag     _writeFlag{ATOMIC_FLAG_INIT};
};

/** enable
 *
 * @brief Enables specified verbosity group.
 *
 * @tparam VGs_T -- VG typename.
 *
 * @param VGs -- Verbosity groups to enable.
 */
template <std::same_as<VG>... VGs_T>
void ym::TextLogger::enable(VGs_T const... VGs)
{
   ((_vGroups.set(VGs)), ...);
}

/** disable
 *
 * @brief Disables specified verbosity group.
 *
 * @tparam VGs_T -- VG typename.
 *
 * @param VG -- Verbosity group to disable.
 */
template <std::same_as<VG>... VGs_T>
void ym::TextLogger::disable(VGs_T const... VGs)
{
   ((_vGroups.clear(VGs)), ...);
}

/** pushEnable
 * 
 * @brief Enables given verbosity group only in the current scope.
 *
 * @tparam VGs_T -- VG typename.
 *
 * @param VG -- Verbosity group.
 * 
 * @returns ScopedEnable -- RAII mechanism that only keeps the enable VG while in scope.
 */
template <std::same_as<VG>... VGs_T>
auto ym::TextLogger::pushEnable([[maybe_unused]] VGs_T const... VGs) -> ScopedEnable
{
   return ScopedEnable(this/*, VGs...*/); // TODO
}

/** printf
 *
 * @brief Prints to the active logger.
 *
 * @throws Whatever print_Handler() throws.
 *
 * @tparam Args_T -- Constrained argument types.
 *
 * @param VG     -- Verbosity level.
 * @param Format -- Format string.
 * @param Args   -- Arguments.
 */
template <typename... Args_T>
inline void TextLogger::printf(
   VG     const VG,
   strlit const Format,
   Args_T &&... args_uref)
{
   printf_Handler(VG, Format, fmt::make_format_args(args_uref...));
}

/** ymLog
 * 
 * @brief Prints to the active logger.
 *
 * @throws Whatever getGlobalInstancePtr() throws.
 * 
 * @tparam Args_T -- Argument types.
 *
 * @param VG     -- Verbosity level.
 * @param Format -- Format string.
 * @param Args   -- Arguments.
 */
template <typename... Args_T>
inline void ymLog(
   VG     const VG,
   strlit const Format,
   Args_T &&... args_uref)
{
   TextLogger::getGlobalInstancePtr()->printf(VG, Format, std::forward<Args_T>(args_uref)...);
}

/** ymLogEnable
 * 
 * @brief Enables specified verbosity group for the global logger.
 *
 * @throws Whatever getGlobalInstancePtr() throws.
 *
 * @tparam VGs_T -- VG typename.
 *
 * @param VG -- Verbosity group to disable.
 */
template <std::same_as<VG>... VGs_T>
inline void ymLogEnable(VGs_T const... VGs)
{
   ((TextLogger::getGlobalInstancePtr()->enable(VGs)), ...);
}

/** ymLogDisable
 * 
 * @brief Disables specified verbosity group for the global logger.
 *
 * @throws Whatever getGlobalInstancePtr() throws.
 *
 * @tparam VGs_T -- VG typename.
 *
 * @param VG -- Verbosity group to disable.
 */
template <std::same_as<VG>... VGs_T>
inline void ymLogDisable(VGs_T const... VGs)
{
   ((TextLogger::getGlobalInstancePtr()->disable(VGs)), ...);
}

/** ymLogPushEnable
 * 
 * @brief Enables given verbosity group only in the current scope for the global logger.
 * 
 * @throws Whatever getGlobalInstancePtr() throws.
 *
 * @tparam VGs_T -- VG typename.
 *
 * @param VG -- Verbosity group.
 * 
 * @returns ScopedEnable -- RAII mechanism that only keeps the enabled VG while in scope.
 */
template <std::same_as<VG>... VGs_T>
inline TextLogger::ScopedEnable ymLogPushEnable(VGs_T const... VGs)
{
   return TextLogger::getGlobalInstancePtr()->pushEnable(VGs...);
}

} // ym
