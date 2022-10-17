/**
 * @file    logger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include "ymception.h"

#include <cstdio>
#include <memory>

namespace ym
{

/** Logger
 *
 * @brief Base class for logging related classes.
 */
class Logger
{
public:
   YM_NO_COPY  (Logger);
   YM_NO_ASSIGN(Logger);

   YM_DECL_YMEXC(LoggerError)

protected:
   explicit Logger(void);

   template <typename... Args_T>
   inline void printfInternalError(str    const    Format,
                                   Args_T const... Args);

   bool openOutfile                (str const Filename);
   bool openOutfile_appendTimeStamp(str const Filename);

   using FileDeleter_T = void(*)(std::FILE * const);
   std::unique_ptr<std::FILE, FileDeleter_T> _outfile_uptr;

private:
   void populateTimeStamp(char * const timeStamp_Ptr,
                          uint64 const TSSize_bytes) const;
};

/** printfInternalError
 *
 * @brief If something goes wrong in the logger then printing to stderr is our
 *        only choice.
 *
 * @tparam Args_T -- Argument types
 *
 * @param Format -- Format string
 * @param Args   -- Arguments
 */
template <typename... Args_T>
inline void Logger::printfInternalError(str    const    Format,
                                        Args_T const... Args)
{
   std::fprintf(stderr, Format, Args...);
}

} // ym
