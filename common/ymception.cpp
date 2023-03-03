/**
 * @author Forrest Jablonski
 */

#include "ymception.h"

#include "memorymanager.h"
#include "textlogger.h"
#include "verbositygroup.h"

#include <boost/stacktrace.hpp>

#include <cstdarg>
#include <cstdio>
#include <cstring>

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
   constexpr auto BufferSize_bytes = 1024_u64;
   auto * const   buffer_Ptr       = MemMgr::allocate_raw<char>(BufferSize_bytes);
   
   std::memset(buffer_Ptr, '\0', BufferSize_bytes);

   // writes null terminator for us
   auto const NCharsWritten = snprintf(buffer_Ptr,
                                       BufferSize_bytes,
                                       "%s \"%s:%u\": ",
                                       Name,
                                       SrcLoc.file_name(),
                                       SrcLoc.line());

   auto const Offset = 
      (                    NCharsWritten  < 0_i32        ) ? 0_u32 : // snprintf encountered an error
      (static_cast<uint32>(NCharsWritten) < BufferSize_bytes) ? static_cast<uint32>(NCharsWritten) :
                                                             BufferSize_bytes;

   std::va_list args;
   va_start(args, Format);

   // writes null terminator for us
   (void)std::vsnprintf(buffer_Ptr + Offset,
                        BufferSize_bytes - Offset,
                        Format,
                        args);

   va_end(args);

   ymLog(VGM_T::Ymception_Assert, "Assert failed!");
   ymLog(VGM_T::Ymception_Assert, "%s!", buffer_Ptr);
   // ymLog(VGM_T::Ymception_Assert, "Stack dump follows...");
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

   return std::unique_ptr<char>(buffer_Ptr);
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
