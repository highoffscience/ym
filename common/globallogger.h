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

   /// @brief Gets this logger's options.
   inline virtual Options_T const & getOptions(void) const noexcept override { return _Options; }

   virtual ~GlobalLogger(void);

   YM_NO_COPY  (GlobalLogger)
   YM_NO_ASSIGN(GlobalLogger)

   YM_DECL_YMASSERT(TextLogger::Error, Error)

   static BoundPtr<GlobalLogger> getGlobalInstance(void);

   /// @brief Returns name of file.
   inline auto getFilename(void) const noexcept { return _Filename; }

   bool isOpen(void) const noexcept;
   bool open  (void) noexcept;
   void close (void) noexcept;

   template <typename... Args_T>
   inline void printf(
      VF const     VFlag,
      strlit       Format,
      Args_T &&... args);

   /** ScopedEnable
    * 
    * @brief Allows managed temporary enabling of a verbosity group.
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
      std::bitset<   NFlags> const _WasEnabled{};
   };

   template <std::same_as<VF>... VFs_T> void enable (VFs_T const... VFlags);
   template <std::same_as<VF>... VFs_T> void disable(VFs_T const... VFlags);

   template <std::same_as<VF>... VFs_T>
   ScopedEnable<VFs_T...> pushEnable(VFs_T const... VFlags);

private:
   explicit GlobalLogger(
      strlit    const   Filename,
      Options_T const & Options = {});

   virtual void producer(
      strlit const     Format,
      fmt::format_args args) noexcept override;

   void printer(void) noexcept;

   /// @brief State of the logger.
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

   /// @brief Stores the message buffer and the synchronization counter.
   struct Slot
   {
      std::array<char, MaxMsgSize_bytes> _msgBuffer{'\0'};
      std::atomic_unsigned_lock_free     _seqN     { 0u };
   };

   static_assert(MaxMsgSize_bytes >= 64uz, "Too limited room"); // time stamps require some space
   static_assert(sizeof(Slot) == SlotSize_bytes, "Slot packing not as expected");

   std::array<Slot, 32uz>         _slots    {  /* default */  };
   strlit    const                _Filename {"unnamed_gl.uhoh"};
   Options_T const                _Options  {  /* default */  };
   VerboGroup                     _vGroup   {  /* default */  };
   std::atomic<State_T>           _state    { State_T::Closed };
   std::atomic_unsigned_lock_free _writePos {        0u       };
   std::atomic_unsigned_lock_free _readPos  {        0u       };
};

/*
 * GlobalLogger member functions.
 * -------------------------------------------------------------------------- */

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

/** printf
 * 
 * @brief Conditionally prints.
 * 
 * @tparam Args_T -- Argument types.
 * 
 * @param VFlag  -- Verbosity flag.
 * @param Format -- Format string.
 * @param args   -- Arguments.
 */
template <typename... Args_T>
inline void ym::GlobalLogger::printf(
   VF const     VFlag,
   strlit       Format,
   Args_T &&... args)
{
   if (VFlag == VF::Errstream)
   { // error printing - print to err stream console
      fmt::print(stderr, "WARNING: ");
      fmt::println(stderr, Format, std::forward<Args_T>(args)...);
   }
   else if (_vGroup.test(VFlag))
   { // verbosity level is enabled - print!
      GlobalLogger::getGlobalInstance()->printf(Format, std::forward<Args_T>(args)...);
   }
}

/*
 * Inner Class ScopedEnable functions.
 * -------------------------------------------------------------------------- */

// template <std::same_as<VF>... VFs_T>
// class ScopedEnable
// {
// public:
//    static constexpr auto NFlags = sizeof...(VFs_T);

//    explicit ScopedEnable(VFs_T const... VFlags);
//    ~ScopedEnable(void);

//    void popEnable(void) const;

// private:
//    std::array<VF, NFlags> const _VFlags;
//    std::bitset<   NFlags> const _WasEnabled{};
// };

// template <std::same_as<VF>... VFs_T>
// ScopedEnable<VFs_T...> pushEnable(VFs_T const... VFlags);

/** ScopedEnable
 * 
 * @brief Constructor.
 * 
 * @note Enables upon construction.
 * 
 * @param logger_Ptr -- Logger instance to enable VG for.
 * @param VG         -- Verbosity group.
 */
template <std::same_as<VF>... VFs_T>
ym::GlobalLogger::ScopedEnable<VFs_T...>::ScopedEnable(VFs_T const... VFlags) :
   _VFlags     {VFlags...},
   _WasEnabled {false}
{

}

/** ~ScopedEnable
 * 
 * @brief Destructor.
 * 
 * @note Disables upon exit.
 */
ym::GlobalLogger::ScopedEnable::~ScopedEnable(void)
{
   popEnable();
}

/** popEnable
 * 
 * @brief Restores the enable state of the stored VG.
 */
void ym::GlobalLogger::ScopedEnable::popEnable(void) const
{
   if (!_WasEnabled)
   { // disable
      _logger_Ptr->disable(_VG);
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
 * @param VF     -- Verbosity flag.
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
