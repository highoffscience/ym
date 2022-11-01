/**
 * @author Forrest Jablonski
 */

#include "ymception.h"

#include "textlogger.h"
#include "verbositygroup.h"

// TODO add boost (apt-get install libboost-all-dev) to cmake build dependencies

#include <boost/stacktrace.hpp>

#include <string>

/** assertHandler
 *
 * @brief Assert has failed. Print diagnostic information and throw.
 */
void ym::Ymception::assertHandler(void) const
{
   ymLog(VGM::Ymception_Assert, "Assert failed!");
   ymLog(VGM::Ymception_Assert, what());
   ymLog(VGM::Ymception_Assert, "Stack dump follows...");

   { // split and print stack dump
      auto const StackDumpStr = boost::stacktrace::to_string(boost::stacktrace::stacktrace());

      for (auto startPos = StackDumpStr.find_first_not_of('\n', 0);
           startPos != std::string::npos;
           /*empty*/)
      { // print each line of the stack dump separately
         auto const EndPos = StackDumpStr.find_first_of('\n', startPos);
         ymLog(VGM::Ymception_Assert, StackDumpStr.substr(startPos, EndPos - startPos).c_str());
         startPos = StackDumpStr.find_first_not_of('\n', EndPos);
      }
   }

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
