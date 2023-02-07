/**
 * @file    logger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "loggable.h"
#include "ymception.h"

#include <cstdio>
#include <memory>
#include <string_view>

namespace ym
{

/** Logger
 *
 * @brief Base class for logging related classes.
 *
 * @note TS = Time Stamp
 *
 * @note We don't pass the file in via the constructor because we don't want to
 *       give the impression we'll try to open it there. We might throw an exception
 *       or the user might want to open multiple files using the same instance.
 */
class Logger
{
public:
   YM_NO_COPY  (Logger)
   YM_NO_ASSIGN(Logger)

   YM_DECL_YMCEPT(LoggerError_UnexpectedFilenameSize)

   /** TimeStampFilenameMode_T
    *
    * @brief Mode to determine appending or not a timestamp to the filename.
    */
   enum class TimeStampFilenameMode_T : uint32
   {
      Append,
      DoNotAppend
   };

   /// @brief Convience alias.
   using TSFM_T = TimeStampFilenameMode_T;

protected:
   explicit Logger(void);

   template <Loggable... Args_T>
   inline void printfInternalError(str    const    Format,
                                   Args_T const... Args);

   inline auto isOutfileOpened(void) const { return static_cast<bool>(_outfile_uptr); }

   // Don't name simply "open" or "close" because we want to allow derived
   // classes to implement these functions without the overhead of
   // virtual calls.

   bool openOutfileToStdout(void);
   bool openOutfile(str    const Filename);
   bool openOutfile(str    const Filename,
                    TSFM_T const TSFilenameMode);

   void closeOutfile(void);

   using FileDeleter_T = void(*)(std::FILE * const);
   std::unique_ptr<std::FILE, FileDeleter_T> _outfile_uptr;

private:
   // TS = Time Stamp
   static constexpr std::string_view s_DefaultTS{"_0000_000_00_00_00_00"};

   bool openOutfile_appendTimeStamp(str const Filename);

   void populateTimeStamp(char * const timeStamp_Ptr) const;
};

/** printfInternalError
 *
 * @brief If something goes wrong in the logger then printing to stderr is our
 *        only choice.
 *
 * @tparam Args_T -- Argument types.
 *
 * @param Format -- Format string.
 * @param Args   -- Arguments.
 */
template <Loggable... Args_T>
inline void Logger::printfInternalError(str    const    Format,
                                        Args_T const... Args)
{
   std::fprintf(stderr, Format, Args...);
}

} // ym
