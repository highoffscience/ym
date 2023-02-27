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
 * TODO std::stacktrace implementation instead of boost
 */
void ym::Ymception::assertHandler(std::source_location const SrcLoc,
                                  str                  const Format,
                                  /*variadic*/               ...)
{
   std::array<char, 1024_u32> buffer{'\0'};

   std::va_list args;
   va_start(args, Format);

   // vsnprintf writes a null terminator for us
   (void)std::vsnprintf(buffer.data(), buffer.size(), Format, args);

   va_end(args);

   ymLog(VGM_T::Ymception_Assert, Format, ...);

   std::va_list args;
   va_start(args, Format);
   
   va_end(args);

   ymLog(VGM_T::Ymception_Assert, "Assert failed!");
   // ymLog(VGM_T::Ymception_Assert, what());
   // ymLog(VGM_T::Ymception_Assert, "Stack dump follows...(pending new implementation)");
   // // ymLog(VGM_T::Ymception_Assert, std::to_string(std::stacktrace::current()).c_str());

   // { // split and print stack dump
   //    auto const StackDumpStr = boost::stacktrace::to_string(boost::stacktrace::stacktrace());

   //    for (auto startPos = StackDumpStr.find_first_not_of('\n', 0);
   //         startPos != std::string::npos;
   //         /*empty*/)
   //    { // print each line of the stack dump separately
   //       auto const EndPos = StackDumpStr.find_first_of('\n', startPos);
   //       ymLog(VGM_T::Ymception_Assert, StackDumpStr.substr(startPos, EndPos - startPos).c_str());
   //       startPos = StackDumpStr.find_first_not_of('\n', EndPos);
   //    }
   // }

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
