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
   enum class FilenameMode_T : uint32
   {
      KeepOriginal,
      AppendTimeStamp
   };

   YM_NO_COPY  (Logger)
   YM_NO_ASSIGN(Logger)

   YM_DECL_YMASSERT(OpenError)

protected:
   explicit Logger(void);

   inline auto isOutfileOpened(void) const { return static_cast<bool>(_outfile_uptr); }

   // Don't name simply "open" or "close" because we want to allow derived
   // classes to implement these functions without the overhead of
   // virtual calls.

   bool openOutfile(
      str            const Filename,
      FilenameMode_T const FilenameMode = FilenameMode_T::AppendTimeStamp);

   void closeOutfile(void);

   using FileDeleter_T = void(*)(std::FILE * const);
   std::unique_ptr<std::FILE, FileDeleter_T> _outfile_uptr;

private:
   bool openOutfile_appendTimeStamp(std::string_view const Filename);
};

} // ym
