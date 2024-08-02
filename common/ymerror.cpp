/**
 * @author Forrest Jablonski
 */

#include "ymerror.h"

#include "textlogger.h"
#include "verbogroup.h"

#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <utility>

#define YM_DISABLE_STACKTRACE

#if !defined(YM_DISABLE_STACKTRACE)
   #include <boost/stacktrace.hpp>
#endif // YM_DISABLE_STACKTRACE

/** Ymerror
 *
 * @brief Constructor.
 *
 * @param msg -- Message string.
 */
ym::Ymerror::Ymerror(std::string  msg,
                         uint32 const Tag)
   : _Msg {std::move(msg)},
     _Tag {Tag           }
{
}

/** assertHandler
 *
 * @brief Assert has failed. Print diagnostic information and throw.
 * 
 * TODO params and returns
 * 
 * @todo std::stacktrace implementation instead of boost.
 */
std::string ym::Ymerror::assertHandler(
   rawstr               const Name,
   std::source_location const SrcLoc,
   rawstr               const Format,
   /*variadic*/               ...)
{
   std::array<char, 1024_u64> buffer{'\0'};

   // writes null terminator for us
   auto const NCharsWritten =
      std::snprintf(buffer.data(),
                    buffer.size(),
                    "%s \"%s:%u\": ",
                    Name,
                    SrcLoc.file_name(),
                    SrcLoc.line());

   auto const Offset = 
      (                    NCharsWritten  < 0            ) ? 0_u32 : // snprintf encountered an error
      (static_cast<uint32>(NCharsWritten) < buffer.size()) ? static_cast<uint32>(NCharsWritten) :
                                                             buffer.size();

   std::va_list args;
   va_start(args, Format);

   // writes null terminator for us
   (void)std::vsnprintf(buffer.data() + Offset,
                        buffer.size() - Offset,
                        Format,
                        args);

   va_end(args);

   // need to check if the global log is the one that threw
   auto const IsGlobalLogOk = std::strcmp(Name, "TextLoggerError_GlobalFailureToOpen") != 0;

   if (!IsGlobalLogOk)
   { // only option is to log to the error stream
      ymLogToStdErr("Assert failed!");
      ymLogToStdErr("%s!", buffer.data());
   }
   else
   { // log the error
      ymLog(VG::Ymerror_Assert, "Assert failed!");
      ymLog(VG::Ymerror_Assert, "%s!", buffer.data());

   #if !defined(YM_DISABLE_STACKTRACE)
      ymLog(VGM_T::Ymerror_Assert, "Stack dump follows...");

      { // split and print stack dump
         auto const StackDumpStr = boost::stacktrace::to_string(boost::stacktrace::stacktrace());

         for (auto startPos = StackDumpStr.find_first_not_of('\n', 0);
            startPos != std::string::npos;
            /*empty*/)
         { // print each line of the stack dump separately
            auto const EndPos = StackDumpStr.find_first_of('\n', startPos);
            ymLog(VGM_T::Ymerror_Assert, StackDumpStr.substr(startPos, EndPos - startPos).c_str());
            startPos = StackDumpStr.find_first_not_of('\n', EndPos);
         }
      }
   #endif // YM_DISABLE_STACKTRACE
   }

   return std::string(buffer.data());
}

/** what
 *
 * @brief Returns the saved off message describing the exception.
 *
 * @returns rawstr -- The saved off message.
 */
auto ym::Ymerror::what(void) const noexcept -> rawstr
{
   return _Msg.c_str();
}
