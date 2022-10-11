/**
 * @author Forrest Jablonski
 */

#include "ymception.h"

#include "objectgroups.h"

#if defined(YM_DBG)
#include <boost/stacktrace.hpp>
#include <string>
#endif // YM_DBG

/**
 * 
 */
void ym::Ymception::assertHandler(void) const
{
   ymLog(OGMask_T::Ymception_Assert, "Assert failed!");
   ymLog(OGMask_T::Ymception_Assert, what());

#if defined(YM_DBG)
   ymLog(0, "Stack dump follows...");

   { // split and print stack dump
      std::string const StackDumpStr = boost::stacktrace::to_string(boost::stacktrace::stacktrace());

      for (auto startPos = StackDumpStr.find_first_not_of('\n', 0);
           startPos != std::string::npos;
           /*empty*/)
      { // print each line of the stack dump separately
         auto const EndPos = StackDumpStr.find_first_of('\n', startPos);
         ymLog(OGMask_T::Ymception_Assert, StackDumpStr.substr(startPos, EndPos - startPos).c_str());
         startPos = StackDumpStr.find_first_not_of('\n', EndPos);
      }
   }
#endif // YM_DBG

   throw *this;
}
