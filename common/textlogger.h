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

#include <string_view>

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
   struct Options_T : public Logger::Options_T
   {
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

      /// @brief Allows direct comparison between Options_T and specified field type.
      friend constexpr bool operator == (Options_T const & Opts, PrintMode_T const Mode) {
         return Opts._printMode == Mode;
      }

      /// @brief Allows direct comparison between OpeningOptions_T and specified field type.
      friend constexpr bool operator == (Options_T const & Opts, RedirectMode_T const Mode) {
         return Opts._redirectMode == Mode;
      }
   };

   /// @brief Returns default options.
   static constexpr Options_T getDefaultOptions(void) { return {}; }
   virtual Options_T const & getOptions(void) const = 0;

   YM_DECL_YMASSERT(Logger::Error, Error)

   /// @brief Returns name of file.
   inline auto getFilename(void) const { return _Filename; }

   /// @brief Prints.
   template <typename... Args_T>
   inline void printf(
         strlit       Format,
         Args_T &&... args) {
      producer(Format, fmt::make_format_args(args...));
   }

protected:
   explicit TextLogger(strlit const Filename);
   virtual ~TextLogger(void) = default;

   YM_NO_COPY  (TextLogger)
   YM_NO_ASSIGN(TextLogger)

   static constexpr std::string_view RawTimeStampTemplate{"uuuuuuuuuuuu"};
   static constexpr std::string_view HumanReadableTimeStampTemplate{"HHH:MM:SS.uuuuuu"};

   mutstr populateFormattedTime(
      mutstr            writePtr,
      std::size_t const BufSize_bytes) const;

   strlit const _Filename{"unnamed.uhoh"};
   Timer        _timer   {/* default */ };
};

} // ym
