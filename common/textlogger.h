/**
 * @file    textlogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "logger.h"
#include "timer.h"
#include "verbogroup.h"
#include "ymglobals.h"

#include "fmt/format.h"

#include <array>
#include <atomic>
#include <utility>

namespace ym
{

/*
 * Convenience functions.
 * -------------------------------------------------------------------------- */

template <typename... Args_T>
inline void ymLog(
   VG     const VG,
   strlit const Format,
   Args_T &&... args_uref);

inline bool ymLogEnable (VG const VG);
inline bool ymLogDisable(VG const VG);

// implemented below - ScopedEnable isn't yet defined
// inline class ScopedEnable ymLogPushEnable(VG const VG);

/* -------------------------------------------------------------------------- */

/** TextLogger
 *
 * @brief Logs text to the given outfile - similary to std::printf.
 */
class TextLogger : public Logger
{
public:
   /** PrintMode_T
    *
    * @brief Mode to determine how to mangle the printable message.
    */
   enum class PrintMode_T : uint32
   {
      KeepOriginal,
      PrependTimeStamp,
      PrependHumanReadableTimeStamp
   };

   /** RedirectMode_T
    * 
    * @brief Specifies what streams to pipe the output to.
    */
   enum class RedirectMode_T : uint32
   {
      ToLog,
      ToLogAndStdOut // for debugging
   };

   explicit TextLogger(
      str            const Filename,
      FilenameMode_T const FilenameMode = FilenameMode_T::AppendTimeStamp,
      PrintMode_T    const PrintMode    = PrintMode_T::PrependHumanReadableTimeStamp,
      RedirectMode_T const RedirectMode = 
         #if (YM_DEBUG || YM_PRINT_TO_SCREEN)
            RedirectMode_T::ToLogAndStdOut
         #else
            RedirectMode_T::ToLog
         #endif
   );
   ~TextLogger(void);

   YM_NO_COPY  (TextLogger)
   YM_NO_ASSIGN(TextLogger)

   YM_DECL_YMASSERT(PrintError)
   YM_DECL_YMASSERT(GlobalError)

   static bptr<TextLogger> getGlobalInstancePtr(void);

   inline auto getPrintMode   (void) const { return _PrintMode;    }
   inline auto getRedirectMode(void) const { return _RedirectMode; }

   bool isOpen(void) const;

   bool open(void);
   void close(void);

   /** ScopedEnable
    * 
    * @brief Allows managed temporary enabling of a verbosity group.
    * 
    * @note Uses RAII to storing/restoring enabling verbosity groups.
    * 
    * @note The return value from pushEnable will need to be explicitly stored, ie.
    *       auto const SE = ymLogPushEnable(VG);
    *       even if SE is not used, since the destructor has side effects. Simply calling
    *       pushEnable will result in the ScopedEnable structure being deleted immediately.
    */
   class ScopedEnable
   {
   public:
      explicit ScopedEnable(
         TextLogger * const logger_Ptr,
         VG           const VG);
      ~ScopedEnable(void);

      void popEnable(void) const;

   private:
      TextLogger * const _logger_Ptr;
      VG           const _VG;
      bool         const _WasEnabled;
   };

   bool enable (VG const VG);
   bool disable(VG const VG);

   ScopedEnable pushEnable(VG const VG);

   template <typename... Args_T>
   inline void printf(
      VG     const VG,
      strlit const Format,
      Args_T &&... args_uref);

private:
   /** State_T
    *
    * @brief State of the logger.
    */
   enum class State_T : uint32
   {
      Closed,
      Closing,
      Open,
      Opening
   };
   
   static constexpr auto _s_MaxMsgSize_bytes = std::size_t(256u);
   static constexpr auto getMaxMsgSize_bytes(void) { return _s_MaxMsgSize_bytes; }
   static constexpr std::string_view RawTimeStampTemplate{"uuuuuuuuuuuu"};
   static constexpr std::string_view HumanReadableTimeStampTemplate{" HHH:MM:SS.uuuuuu: "};

   static_assert(_s_MaxMsgSize_bytes >= std::size_t(64u), "Too limited room");

   void acquireWriteAccess(void);
   void releaseWriteAccess(void);

   void printf_Handler(
      VG     const     VG,
      strlit const     Format,
      fmt::format_args args);

   void printf_Handler(
      strlit const     Format,
      fmt::format_args args);

   char * populateFormattedTime(char * write_ptr) const;

   using VGroups_T = std::array<std::atomic<uint8>, VerboGroup::getNGroups()>;

   static inline TextLogger * _s_globalInstance_ptr{nullptr};

   PrintMode_T    const _PrintMode   {PrintMode_T::KeepOriginal};
   RedirectMode_T const _RedirectMode{RedirectMode_T::ToLog    };
   VGroups_T            _vGroups     {      /* default */      };
   Timer                _timer       {      /* default */      };
   std::atomic<State_T> _state       {State_T::Closed          };
   std::atomic_flag     _writeFlag   {ATOMIC_FLAG_INIT         };
};

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
 * @param VG -- Verbosity group to enable.
 * 
 * @returns bool -- If previously enabled before this call.
 */
inline bool ymLogEnable(VG const VG)
{
   return TextLogger::getGlobalInstancePtr()->enable(VG);
}

/** ymLogDisable
 * 
 * @brief Disables specified verbosity group for the global logger.
 *
 * @throws Whatever getGlobalInstancePtr() throws.
 *
 * @param VG -- Verbosity group to disable.
 * 
 * @returns bool -- If previously enabled before this call.
 */
inline bool ymLogDisable(VG const VG)
{
   return TextLogger::getGlobalInstancePtr()->disable(VG);
}

/** ymLogPushEnable
 * 
 * @brief Enables given verbosity group only in the current scope for the global logger.
 * 
 * @throws Whatever getGlobalInstancePtr() throws.
 * 
 * @param VG -- Verbosity group.
 * 
 * @returns ScopedEnable -- RAII mechanism that only keeps the enable VG while in scope.
 */
inline TextLogger::ScopedEnable ymLogPushEnable(VG const VG)
{
   return TextLogger::getGlobalInstancePtr()->pushEnable(VG);
}

} // ym
