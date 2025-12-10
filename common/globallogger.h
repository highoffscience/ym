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
 * Global convenience functions.
 * -------------------------------------------------------------------------- */

template <typename... Args_T>
inline void ymLog(
   VF     const VFlag,
   strlit const Format,
   Args_T &&... args);

template <std::same_as<VF>... VFs_T> inline void ymLogEnable (VFs_T const... VFlags);
template <std::same_as<VF>... VFs_T> inline void ymLogDisable(VFs_T const... VFlags);

// template <std::same_as<VF>... VFs_T>
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

   /// @brief Returns default options.
   static constexpr Options_T getDefaultOptions(void) { return {}; }

   /// @brief Gets this logger's options.
   inline virtual Options_T const & getOptions(void) const override { return _Options; }

   virtual ~GlobalLogger(void);

   YM_NO_COPY  (GlobalLogger)
   YM_NO_ASSIGN(GlobalLogger)

   YM_DECL_YMASSERT(PrintError)
   YM_DECL_YMASSERT(GlobalError)

   static BoundPtr<GlobalLogger> getGlobalInstance(void);

   template <typename... Args_T>
   inline void printf(
      VF const     VFlag,
      strlit       Format,
      Args_T &&... args);

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
      std::array<char, MaxMsgSize_bytes> _msgBuffer{  };
      std::atomic_unsigned_lock_free     _seqN     {0u};
   };

   static_assert(MaxMsgSize_bytes >= 64uz, "Too limited room"); // time stamps require some space
   static_assert(sizeof(Slot) == SlotSize_bytes, "Slot packing not as expected");

   std::array<Slot, 32uz>         _slots    { /* default */  };
   Options_T const                _Options  { /* default */  };
   VerboGroup                     _vGroup   { /* default */  };
   std::atomic<State_T>           _state    {State_T::Closed };
   std::atomic_unsigned_lock_free _writePos {       0u       };
   std::atomic_unsigned_lock_free _readPos  {       0u       };
   std::atomic_flag               _writeFlag{ATOMIC_FLAG_INIT};
};

/** enable
 *
 * @brief Enables specified verbosity flags.
 *
 * @tparam VFs_T -- VF typename.
 *
 * @param VFlags -- Verbosity flags to enable.
 */
template <std::same_as<VF>... VFs_T>
void ym::GlobalLogger::enable(VFs_T const... VFlags)
{
   ((_vGroup.set(VFlags)), ...);
}

/** disable
 *
 * @brief Disables specified verbosity flags.
 *
 * @tparam VFs_T -- VF typename.
 *
 * @param VFlags -- Verbosity flags to disable.
 */
template <std::same_as<VF>... VFs_T>
void ym::GlobalLogger::disable(VFs_T const... VFlags)
{
   ((_vGroup.clear(VFlags)), ...);
}

/** pushEnable
 * 
 * @brief Enables given verbosity group only in the current scope.
 *
 * @tparam VFs_T -- VF typename.
 *
 * @param VFlags -- Verbosity flags to enable.
 * 
 * @returns ScopedEnable -- RAII mechanism that only keeps the enable VF while in scope.
 */
template <std::same_as<VF>... VFs_T>
auto ym::GlobalLogger::pushEnable(VFs_T const... VFlags) -> ScopedEnable<VFs_T...>
{
   return ScopedEnable(VFlags...);
}

/**
 * @brief TODO
 * 
 * @tparam VFs_T 
 * @tparam Args_T 
 * 
 * @param VFlags 
 * @param Format 
 * @param args 
 */
template <typename... Args_T>
inline void ym::GlobalLogger::printf(
   VF const     VFlag,
   strlit       Format,
   Args_T &&... args)
{
   if (_vGroup.test(VFlag))
   { // verbosity level is enabled - print!
      GlobalLogger::getGlobalInstance()->printf(VFlag, Format, std::forward<Args_T>(args)...);
   }
}

/*
 * Global convenience functions.
 * -------------------------------------------------------------------------- */

/** ymLog
 * 
 * @brief Prints to the active logger.
 *
 * @throws Whatever getGlobalInstance() throws.
 * 
 * @tparam Args_T -- Argument types.
 *
 * @param VF     -- Verbosity level.
 * @param Format -- Format string.
 * @param Args   -- Arguments.
 */
template <typename... Args_T>
inline void ymLog(
   VF     const VFlag,
   strlit const Format,
   Args_T &&... args)
{
   GlobalLogger::getGlobalInstance()->printf(VFlag, Format, std::forward<Args_T>(args)...);
}

/** ymLogEnable
 * 
 * @brief Enables specified verbosity group for the global logger.
 *
 * @throws Whatever getGlobalInstance() throws.
 *
 * @tparam VFs_T -- VF typename.
 *
 * @param VF -- Verbosity flag to enable.
 */
template <std::same_as<VF>... VFs_T>
inline void ymLogEnable(VFs_T const... VFlags)
{
   GlobalLogger::getGlobalInstance()->enable(VFlags...);
}

/** ymLogDisable
 * 
 * @brief Disables specified verbosity group for the global logger.
 *
 * @throws Whatever getGlobalInstance() throws.
 *
 * @tparam VFs_T -- VF typename.
 *
 * @param VF -- Verbosity flag to disable.
 */
template <std::same_as<VF>... VFs_T>
inline void ymLogDisable(VFs_T const... VFlags)
{
   GlobalLogger::getGlobalInstance()->disable(VFlags...);
}

/** ymLogPushEnable
 * 
 * @brief Enables given verbosity group only in the current scope for the global logger.
 * 
 * @throws Whatever getGlobalInstance() throws.
 *
 * @tparam VFs_T -- VF typename.
 *
 * @param VF -- Verbosity flag.
 * 
 * @returns ScopedEnable -- RAII mechanism that only keeps the enabled VF while in scope.
 */
template <std::same_as<VF>... VFs_T>
inline GlobalLogger::ScopedEnable<VFs_T...> ymLogPushEnable(VFs_T const... VFlags)
{
   return GlobalLogger::getGlobalInstance()->pushEnable(VFlags...);
}

} // ym
