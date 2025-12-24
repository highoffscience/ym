/**
 * @file    logger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include "fmt/base.h"

#include <cstdio>
#include <string_view>

namespace ym
{

/** Logger
 *
 * @brief Base class for logging related classes.
 *
 * @note We don't pass the file in via the constructor because we don't want to
 *       give the impression we'll try to open it there. We might throw an exception
 *       or the user might want to open multiple files using the same instance.
 */
class Logger
{
public:
   /** FilenameMode_T
    *
    * @brief Mode to determine how to mangle the filename.
    */
   enum class FilenameMode_T
   {
      KeepOriginal,
      AppendTimeStamp
   };

   /** OverwriteMode
    * 
    * @brief Mode to indicate if file can be overwritten.
    */
   enum class OverwriteMode_T
   {
      Allow,
      Disallow
   };

   /** Options_T
    * 
    * @brief Options surrounding opening a file.
    */
   struct Options_T
   {
      /// @brief Mode to determine how to mangle the filename.
      FilenameMode_T _filenameMode{FilenameMode_T::AppendTimeStamp};

      /// @brief Mode to determine if to overwrite file while opening or not.
      OverwriteMode_T _overwriteMode{OverwriteMode_T::Disallow};

      /// @name Comparison operations.
      /// @{
      /// @brief Allows direct comparison between Options_T and specified field type.
      friend constexpr bool operator == (Options_T const & Opts, FilenameMode_T const Mode) noexcept {
         return Opts._filenameMode == Mode;
      }
      friend constexpr bool operator == (Options_T const & Opts, OverwriteMode_T const Mode) noexcept {
         return Opts._overwriteMode == Mode;
      }
      /// @}
   };

   virtual Options_T const & getOptions(void) const noexcept = 0;

   YM_NO_COPY  (Logger)
   YM_NO_ASSIGN(Logger)

   YM_DECL_YMASSERT(Error)

   /// @brief Prints.
   template <typename... Args_T>
   inline void printf(
         strlit       Format,
         Args_T &&... args) {
      producer(Format, fmt::make_format_args(args...));
   }

protected:
   explicit Logger(void) noexcept = default;
   virtual ~Logger(void) noexcept;

   virtual void producer(
      strlit const     Format,
      fmt::format_args args) noexcept = 0;

   /// @brief Returns if outfile is opened.
   inline auto isOutfileOpened(void) const noexcept { return _file != nullptr; }

   // Don't name simply "open" or "close" because we want to allow derived
   // classes to implement these functions without the overhead of
   // virtual calls.

   bool openOutfile(std::string_view const Filename, Options_T const & Options) noexcept;
   void closeOutfile(void) noexcept;
   
   FreePtr<std::FILE> _file{nullptr};

private:
   void openOutfile_core           (std::string_view const Filename, Options_T const & Options) noexcept;
   void openOutfile_appendTimeStamp(std::string_view const Filename, Options_T const & Options) noexcept;
};

} // ym
