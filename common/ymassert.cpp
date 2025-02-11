/**
 * @file    ymassert.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymassert.h"

std::string ym::ymassert_Base::handler(
   rawstr const     Format,
   fmt::format_args args)
{
   std::string msg(getMaxMsgSize_bytes(), '\0');

   auto const Result = fmt::vformat_to_n(
      _msg.data(),
      _msg.size() - std::size_t(1),
      Format,
      args);
      
   *Result.out = '\0';

   return msg;
}

// /** ymassert_Base
//  *
//  * @brief Constructor.
//  *
//  * @note Assert has failed. Print diagnostic information and throw/flag error.
//  * 
//  * @todo std::stacktrace.
//  *
//  * @param SrcLoc -- File location information.
//  * @param Format -- Format string.
//  * @param ...    -- Arguments.
//  */
// ym::ymassert_Base::ymassert_Base(
//    YM_HELPER_SRC_LOC_PRM(SrcLoc)
//    rawstr          const Format,
//    /*variadic*/          ...)
// {
//    // let's say Format is "Index is %u"
//    // I want "Ymassert @ \"%s:%u\": Index is %u"

//    auto const Prefix = "Ymassert @ \"%s:%u\": "_str;
//    auto const PrefixSize_bytes = std::strlen(Prefix);
//    auto const FormatSize_bytes = std::strlen(Format);
//    auto const TotalSize_bytes  = PrefixSize_bytes + FormatSize_bytes;

//    auto * const buffer_Ptr = YM_STACK_ALLOC(char, TotalSize_bytes + 1_u64); // +1 for null terminator

//    std::strcpy(buffer_Ptr, Prefix);
//    std::strcpy(buffer_Ptr + PrefixSize_bytes, Format);
//    buffer_Ptr[TotalSize_bytes] = '\0'; // safety, though strcpy does this for us

//    std::va_list args;

//    auto argsDeleter = [] (std::va_list * const args_Ptr) {
//       va_end(*args_Ptr);
//    };
//    auto args_uptr = std::unique_ptr<std::va_list *, decltype(argsDeleter)>(&args, argsDeleter);

//    va_start(args, Format);

//    ymLog(VG::YmError_Assert, buffer_Ptr, args);

//    // std::array<char, 512_u32> buffer{'\0'};

//    // ymLog(VG::YmError_Assert,
//    //    #if (YM_CPP_STANDARD >= 20)
//    //       "Ymassert @ \"%s:%u\": ",
//    //       SrcLoc.file_name(),
//    //       static_cast<uint32>(SrcLoc.line())
//    //    #else
//    //       "%s: ",
//    //       Name
//    //    #endif
//    // );

//    // std::array<char, 1024_u64> buffer{'\0'};

//    // // writes null terminator for us
//    // auto const NCharsWritten =
//    //    std::snprintf(
//    //       buffer.data(),
//    //       buffer.size(),
//    //    #if (YM_CPP_STANDARD >= 20)
//    //       "%s \"%s:%u\": ",
//    //       Name,
//    //       SrcLoc.file_name(),
//    //       static_cast<uint32>(SrcLoc.line())
//    //    #else
//    //       "%s: ",
//    //       Name
//    //    #endif
//    //       );

//    // auto const Offset = 
//    //    (                    NCharsWritten  < 0            ) ? 0_u32 : // snprintf encountered an error
//    //    (static_cast<uint32>(NCharsWritten) < buffer.size()) ? static_cast<uint32>(NCharsWritten) :
//    //                                                           buffer.size();

//    // std::va_list args;
//    // va_start(args, Format);

//    // // writes null terminator for us
//    // (void)std::vsnprintf(buffer.data() + Offset,
//    //                      buffer.size() - Offset,
//    //                      Format,
//    //                      args);

//    // va_end(args);

//    // // need to check if the global log is the one that threw
//    // auto const IsGlobalLogOk = std::strcmp(Name, "TextLoggerError_GlobalFailureToOpen") != 0;

//    // if (!IsGlobalLogOk)
//    // { // only option is to log to the error stream
//    //    ymLogToStdErr("Assert failed!");
//    //    ymLogToStdErr("%s!", buffer.data());
//    // }
//    // else
//    // { // log the error
//    //    ymLog(VG::YmError_Assert, "Assert failed!");
//    //    ymLog(VG::YmError_Assert, "%s!", buffer.data());
//    // }
// }

#if (YM_YES_EXCEPTIONS)

/** what
 *
 * @brief Returns an identifying message.
 */
auto ym::ymassert_Base::what(void) const noexcept -> rawstr
{
   return "Ymassert";
}

#endif
