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
#include <string>

/** assertHandler
 *
 * @brief Assert has failed. Print diagnostic information and throw.
 * 
 * @note std::snprintf writes a null character for us.
 * 
 * TODO std::stacktrace implementation instead of boost.
 */
void ym::Ymception::assertHandler(str                  const Name,
                                  std::source_location const SrcLoc,
                                  str                  const Format,
                                  /*variadic*/               ...)
{
   std::array<char, 1024_u32> buffer{'\0'};

   auto const NCharsWritten = snprintf(buffer.data(),
                                       buffer.size(),
                                       "%s <%s:%u>: ",
                                       Name,
                                       SrcLoc.file_name(),
                                       SrcLoc.line());

   auto const Offset = (NCharsWritten < 0_i32        ) ? 0_u32 : // snprintf encountered an error
                       (static_cast<uint32>(NCharsWritten) < buffer.size()) ? static_cast<uint32>(NCharsWritten) :
                                                         buffer.size();

   std::va_list args;
   va_start(args, Format);

   (void)std::vsnprintf(buffer.data() + Offset,
                        buffer.size() - Offset,
                        Format,
                        args);

   va_end(args);

   ymLog(VGM_T::Ymception_Assert, "Assert failed!");
   ymLog(VGM_T::Ymception_Assert, "%s!", buffer.data());
   ymLog(VGM_T::Ymception_Assert, "Stack dump follows...");
   // ymLog(VGM_T::Ymception_Assert, std::to_string(std::stacktrace::current()).c_str());

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

   // TODO make this throw!
   // TODO also format the line </home/forrest/code/ym/common/argparser.cpp:117>
   //      so it hyperlinks in vscode correctly! eg
   //      "/home/forrest/code/ym/unittests/ym/common/argparser/testsuite.py", line 73
   // throw *this;
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
