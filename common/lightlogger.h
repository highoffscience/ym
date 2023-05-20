/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ymdefs.h"

#include "logger.h"

namespace ym
{

/** LightLogger
 * 
 * @brief Basic logger, nothing fancy.
 */
class LightLogger : public Logger
{
public:
   explicit LightLogger(void) = default;

   YM_NO_COPY  (LightLogger);
   YM_NO_ASSIGN(LightLogger);

   inline bool isOpen(void) const { return isOutfileOpened(); }

   inline bool open(std::FILE *    const file_Ptr) { return openOutfile(file_Ptr); }
   inline bool open(str            const Filename,
                    FilenameMode_T const FilenameMode = FilenameMode_T::KeepOriginal) {
      return openOutfile(Filename, FilenameMode);
   }

   template <Loggable... Args_T>
   inline void printf(str    const    Format,
                      Args_T const... Args);
};

/** printf
 *
 * @brief Prints format string to the active stream.
 *
 * @tparam Args_T -- Constrained argument types.
 *
 * @param Format -- Format string.
 * @param Args   -- Arguments.
 */
template <Loggable... Args_T>
inline void LightLogger::printf(str    const    Format,
                                Args_T const... Args)
{
   std::fprintf(_outfile_uptr.get(), Format, Args...);
}

} // ym
