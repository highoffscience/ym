/**
 * @author Forrest Jablonski
 */

#include "ymception.h"

#include "textlogger.h"
#include "verbositygroup.h"

#include <string>

/** assertHandler
 *
 * @brief Assert has failed. Print diagnostic information and throw.
 */
void ym::Ymception::assertHandler(void) const
{
   ymLog(VGM_T::Ymception_Assert, "Assert failed!");
   ymLog(VGM_T::Ymception_Assert, what());
   ymLog(VGM_T::Ymception_Assert, "Stack dump follows...(todo pending)");

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

   throw *this;
}

/** what
 *
 * @brief Returns the saved off message describing the exception.
 *
 * @return str -- The saved off message
 */
auto ym::Ymception::what(void) const noexcept -> str
{
   return _msg.data();
}
