/**
 * @file    logger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ymglobals.h"

#include <cstdio>
#include <memory>
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

   /** OpeningOptions_T
    * 
    * @brief Options surrounding opening a file.
    */
   struct OpeningOptions_T
   {
      /// @brief Mode to determine how to mangle the filename.
      FilenameMode_T _filenameMode{FilenameMode_T::AppendTimeStamp};

      /// @brief Mode to determine if to overwrite file while opening or not.
      OverwriteMode_T _overwriteMode{OverwriteMode_T::Disallow};

      /// @brief Allows direct comparison between OpeningOptions_T and specified field type.
      constexpr friend bool operator == (OpeningOptions_T const & Opts, FilenameMode_T const Mode) {
         return Opts._filenameMode == Mode;
      }

      /// @brief Allows direct comparison between OpeningOptions_T and specified field type.
      constexpr friend bool operator == (OpeningOptions_T const & Opts, OverwriteMode_T const Mode) {
         return Opts._overwriteMode == Mode;
      }
   };

   static constexpr OpeningOptions_T getDefaultOpeningOptions(void) { return {}; }

   YM_NO_COPY  (Logger)
   YM_NO_ASSIGN(Logger)

   YM_DECL_YMASSERT(OpenError)

protected:
   explicit Logger(void);

   inline auto isOutfileOpened(void) const { return static_cast<bool>(_outfile_uptr); }

   // Don't name simply "open" or "close" because we want to allow derived
   // classes to implement these functions without the overhead of
   // virtual calls.

   bool openOutfile(std::string_view const Filename, OpeningOptions_T const & Options);
   void closeOutfile(void);
   
   using FileDeleter_T = void(*)(std::FILE * const);
   std::unique_ptr<std::FILE, FileDeleter_T> _outfile_uptr;

private:
   void openOutfile_core           (std::string_view const Filename, OpeningOptions_T const & Options);
   void openOutfile_appendTimeStamp(std::string_view const Filename, OpeningOptions_T const & Options);
};

} // ym
