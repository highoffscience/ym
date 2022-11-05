/**
 * @file    logger.h
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#pragma once

#include "ym.h"

#include <cstdio>
#include <memory>
#include <string_view>
#include <type_traits>

namespace ym
{

/** Loggable_T
 *
 * @brief Represents a supported loggable data type.
 *
 * @ref <https://en.cppreference.com/w/cpp/io/c/fprintf>
 *
 * @tparam T -- Type that is loggable
 */
template <typename T>
concept Loggable_T = std::is_fundamental_v<T> ||
                     std::is_pointer_v    <T> ;

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
   YM_NO_COPY  (Logger);
   YM_NO_ASSIGN(Logger);

   YM_DECL_YMEXC(LoggerError)

protected:
   explicit Logger(void);

   template <Loggable_T... Args_T>
   inline void printfInternalError(str    const    Format,
                                   Args_T const... Args);

   inline auto isOutfileOpened(void) const { return static_cast<bool>(_outfile_uptr); }

   // Don't name simply "open" or "close" because we want to allow derived
   // classes to implement these functions without the overhead of
   // virtual calls.

   bool openOutfile                (str const Filename);
   bool openOutfile_appendTimeStamp(str const Filename);

   void closeOutfile(void);

   using FileDeleter_T = void(*)(std::FILE * const);
   std::unique_ptr<std::FILE, FileDeleter_T> _outfile_uptr;

private:
   // TS = Time Stamp
   static constexpr std::string_view s_DefaultTS{"_0000_000_00_00_00_00"};

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
template <Loggable_T... Args_T>
inline void Logger::printfInternalError(str    const    Format,
                                        Args_T const... Args)
{
   std::fprintf(stderr, Format, Args...);
}

} // ym
