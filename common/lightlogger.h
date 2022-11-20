/**
 * @author Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "logger.h"

namespace ym
{

/**
 * TODO add isOpen, close
 */
class LightLogger : public Logger
{
public:
   explicit LightLogger(void) = default;

   YM_NO_COPY  (LightLogger);
   YM_NO_ASSIGN(LightLogger);

   inline bool open(str const Filename) { return openOutfile(Filename); }

   template <Loggable... Args_T>
   inline void printf(str    const    Format,
                      Args_T const... Args);
};

/**
 *
 */
template <Loggable... Args_T>
inline void LightLogger::printf(str    const    Format,
                                Args_T const... Args)
{
   std::fprintf(_outfile_uptr.get(), Format, Args...);
}

} // ym
