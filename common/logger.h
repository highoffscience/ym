/**
 * @file    logger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <cstdio>
#include <memory>

namespace ym
{

/**
 * Base class for logging related classes.
 */
class Logger
{
public:
   YM_NO_COPY  (Logger);
   YM_NO_ASSIGN(Logger);

protected:
   explicit Logger(void);

   template <typename... Args_T>
   inline void printfError(str    const    Format,
                           Args_T const... Args);

   bool openOutfile                (str const Filename);
   bool openOutfile_appendTimeStamp(str const Filename);

   using FileDeleter_T = void(*)(std::FILE * const);
   std::unique_ptr<std::FILE, FileDeleter_T> _outfile_uptr;
};

/**
 * If something goes wrong in the logger then printing to stderr is our
 *  only choice.
 *
 * @param Format
 * @param Args
 */
template <typename... Args_T>
inline void Logger::printfError(str    const    Format,
                                Args_T const... Args)
{
   std::fprintf(stderr, Format, Args...);
}

} // ym
