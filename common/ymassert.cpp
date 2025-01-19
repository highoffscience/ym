/**
 * @file    ymassert.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymassert.h"

#include "textlogger.h"

#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <utility>

#define YM_ENABLE_STACKTRACE 0

#if YM_ENABLE_STACKTRACE
   #include <boost/stacktrace.hpp>
#endif

/** ymassert_Base
 *
 * @brief Constructor.
 *
 * @note Assert has failed. Print diagnostic information and throw/flag error.
 * 
 * @todo std::stacktrace implementation instead of boost.
 *
 * @param SrcLoc -- File location information.
 * @param Format -- Format string.
 * @param ...    -- Arguments.
 */
ym::ymassert_Base::ymassert_Base(
   #if (YM_CPP_STANDARD >= 20)
      std::source_location const SrcLoc,
   #endif
      rawstr               const Format,
      /*variadic*/               ...)
{
   ymLog(VG::YmError_Assert,
      #if (YM_CPP_STANDARD >= 20)
         "Ymassert @ \"%s:%u\": ",
         SrcLoc.file_name(),
         static_cast<uint32>(SrcLoc.line())
      #else
         "%s: ",
         Name
      #endif
   );

   std::array<char, 1024_u64> buffer{'\0'};

   // writes null terminator for us
   auto const NCharsWritten =
      std::snprintf(
         buffer.data(),
         buffer.size(),
      #if (YM_CPP_STANDARD >= 20)
         "%s \"%s:%u\": ",
         Name,
         SrcLoc.file_name(),
         static_cast<uint32>(SrcLoc.line())
      #else
         "%s: ",
         Name
      #endif
         );

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
      ymLog(VG::YmError_Assert, "Assert failed!");
      ymLog(VG::YmError_Assert, "%s!", buffer.data());

   #if (YM_ENABLE_STACKTRACE)
      ymLog(VGM_T::YmError_Assert, "Stack dump follows...");

      { // split and print stack dump
         auto const StackDumpStr = boost::stacktrace::to_string(boost::stacktrace::stacktrace());

         for (auto startPos = StackDumpStr.find_first_not_of('\n', 0);
            startPos != std::string::npos;
            /*empty*/)
         { // print each line of the stack dump separately
            auto const EndPos = StackDumpStr.find_first_of('\n', startPos);
            ymLog(VGM_T::YmError_Assert, StackDumpStr.substr(startPos, EndPos - startPos).c_str());
            startPos = StackDumpStr.find_first_not_of('\n', EndPos);
         }
      }
   #endif
   }

   return std::string(buffer.data());
}

/** what
 *
 * @brief Returns an identifying message.
 */
auto ym::ymassert_Base::what(void) const noexcept -> rawstr
{
   return "Ymassert";
}

/** assertHandler
 *
 * @brief Assert has failed. Print diagnostic information and throw.
 * 
 * @todo std::stacktrace implementation instead of boost.
 * 
 * @param Name   -- Name of YmError being thrown.
 * @param SrcLoc -- Source location of the thrown YmError.
 * @param Format -- Format string.
 * @param ...    -- Arguments.
 * 
 * @returns std::string -- YmError formatted details.
 */
std::string ym::YmError::assertHandler(
   rawstr               const Name,
#if (YM_CPP_STANDARD >= 20)
   std::source_location const SrcLoc,
#endif
   rawstr               const Format,
   /*variadic*/         ...)
{
   std::array<char, 1024_u64> buffer{'\0'};

   // writes null terminator for us
   auto const NCharsWritten =
      std::snprintf(
         buffer.data(),
         buffer.size(),
      #if (YM_CPP_STANDARD >= 20)
         "%s \"%s:%u\": ",
         Name,
         SrcLoc.file_name(),
         static_cast<uint32>(SrcLoc.line())
      #else
         "%s: ",
         Name
      #endif
         );

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
      ymLog(VG::YmError_Assert, "Assert failed!");
      ymLog(VG::YmError_Assert, "%s!", buffer.data());

   #if YM_ENABLE_STACKTRACE
      ymLog(VGM_T::YmError_Assert, "Stack dump follows...");

      { // split and print stack dump
         auto const StackDumpStr = boost::stacktrace::to_string(boost::stacktrace::stacktrace());

         for (auto startPos = StackDumpStr.find_first_not_of('\n', 0);
            startPos != std::string::npos;
            /*empty*/)
         { // print each line of the stack dump separately
            auto const EndPos = StackDumpStr.find_first_of('\n', startPos);
            ymLog(VGM_T::YmError_Assert, StackDumpStr.substr(startPos, EndPos - startPos).c_str());
            startPos = StackDumpStr.find_first_not_of('\n', EndPos);
         }
      }
   #endif // YM_ENABLE_STACKTRACE
   }

   return std::string(buffer.data());
}
