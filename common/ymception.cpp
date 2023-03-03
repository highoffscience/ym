/**
 * @author Forrest Jablonski
 */

#include "ymception.h"

#include "textlogger.h"
#include "verbositygroup.h"

#include <boost/stacktrace.hpp>

#include <array>
#include <cstdarg>
#include <cstdio>

// TODO does this belong additionally in ym.h?
#define YM_EXC_DISABLE_STACKTRACE

/** assertHandler
 *
 * @brief Assert has failed. Print diagnostic information and throw.
 * 
 * TODO std::stacktrace implementation instead of boost.
 */
std::string ym::Ymception::assertHandler(str                  const Name,
                                         std::source_location const SrcLoc,
                                         str                  const Format,
                                         /*variadic*/               ...)
{
   std::array<char, 1024_u64> buffer{'\0'};

   // writes null terminator for us
   auto const NCharsWritten = snprintf(buffer.data(),
                                       buffer.size(),
                                       "%s \"%s:%u\": ",
                                       Name,
                                       SrcLoc.file_name(),
                                       SrcLoc.line());

   auto const Offset = 
      (                    NCharsWritten  < 0_i32        ) ? 0_u32 : // snprintf encountered an error
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

   ymLog(VGM_T::Ymception_Assert, "Assert failed!");
   ymLog(VGM_T::Ymception_Assert, "%s!", buffer.data());
   ymLog(VGM_T::Ymception_Assert, "Stack dump follows...");

   { // split and print stack dump
      auto const StackDumpStr = boost::stacktrace::to_string(boost::stacktrace::stacktrace());

      for (auto startPos = StackDumpStr.find_first_not_of('\n', 0);
           startPos != std::string::npos;
           /*empty*/)
      { // print each line of the stack dump separately
         auto const EndPos = StackDumpStr.find_first_of('\n', startPos);
         ymLog(VGM_T::Ymception_Assert, StackDumpStr.substr(startPos, EndPos - startPos).c_str());
         startPos = StackDumpStr.find_first_not_of('\n', EndPos);
      }
   }

   return std::string(buffer.data());
}

/** what
 *
 * @brief Returns the saved off message describing the exception.
 *
 * @return str -- The saved off message
 */
auto ym::Ymception::what(void) const noexcept -> str
{
   return _Msg.c_str();
}
