/**
 * @file    textlogger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "logger.h"
#include "timer.h"
#include "ymglobals.h"

#include "fmt/base.h"

#include <array>
#include <atomic>
#include <concepts>
#include <string_view>
#include <utility>

namespace ym
{

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
   enum class PrintMode_T
   {
      KeepOriginal,
      PrependTimeStamp,
      PrependHumanReadableTimeStamp
   };

   /** RedirectMode_T
    * 
    * @brief Specifies what streams to pipe the output to.
    */
   enum class RedirectMode_T
   {
      ToLog,
      ToLogAndStdOut // for debugging
   };

   /** Options_T
    * 
    * @brief Options surrounding opening and writing to a file.
    */
   struct Options_T
   {
      /// @brief Opening options (defined in base Logger).
      Logger::Options_T _baseOptions{Logger::getDefaultOptions()};

      /// @brief Mode to determine how to mangle the printable message.
      PrintMode_T _printMode{PrintMode_T::PrependHumanReadableTimeStamp};

      /// @brief Mode to specify what streams to pipe the output to.
      RedirectMode_T _redirectMode{
         #if (YM_DEBUG || YM_PRINT_TO_SCREEN)
            RedirectMode_T::ToLogAndStdOut
         #else
            RedirectMode_T::ToLog
         #endif
      };

      /// @brief Convenience cast to pass to base Logger functions.
      constexpr operator Logger::Options_T(void) const { return _baseOptions; }

      /// @brief Allows direct comparison between Options_T and specified field type.
      friend constexpr bool operator == (Options_T const & Opts, PrintMode_T const Mode) {
         return Opts._printMode == Mode;
      }

      /// @brief Allows direct comparison between OpeningOptions_T and specified field type.
      friend constexpr bool operator == (Options_T const & Opts, RedirectMode_T const Mode) {
         return Opts._redirectMode == Mode;
      }
   };

   static constexpr Options_T getDefaultOptions(void) { return {}; }

   explicit TextLogger(strlit const Filename);
   virtual ~TextLogger(void);

   YM_NO_COPY  (TextLogger)
   YM_NO_ASSIGN(TextLogger)

   YM_DECL_YMASSERT(Error)

   inline auto         getFilename(void) const { return _Filename; }
   inline auto const & getOptions (void) const { return _options;  }

   bool isOpen(void) const;

   bool open(void);
   void close(void);

   template <typename... Args_T>
   inline void printf(
         strlit       Format,
         Args_T &&... args) {
      producer(Format, fmt::make_format_args(args...));
   }

protected:
   virtual void producer(
      strlit const     Format,
      fmt::format_args args) = 0;

protected:
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

   static constexpr std::string_view RawTimeStampTemplate{"uuuuuuuuuuuu"};
   static constexpr std::string_view HumanReadableTimeStampTemplate{"HHH:MM:SS.uuuuuu"};

   mutstr populateFormattedTime(
      mutstr      writePtr,
      sizet const BufSize_bytes) const;

   strlit    const      _Filename{"unnamed.uhoh"     };
   Options_T            _options {getDefaultOptions()};
   Timer                _timer   {   /* default */   };
   std::atomic<State_T> _state   {State_T::Closed    };
};

} // ym
