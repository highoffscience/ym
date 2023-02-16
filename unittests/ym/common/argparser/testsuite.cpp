/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ym.h"
#include "testsuite.h"

#include "argparser.h"
#include "textlogger.h"

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void)
   : TestSuiteBase("ArgParser")
{
   addTestCase<BasicParse>();
}

/** run
 *
 * @brief Tests if ArgParser can parse.
 *
 * @return DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::BasicParse::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   TextLogger::getGlobalInstance()->enable(VGM_T::Ymception);

   using Arg = ArgParser::Arg;

   auto excHappened = false;

   try
   {
      ArgParser argparser({
         Arg("")
      });
   }
   catch(Ymception const & E)
   {
      excHappened = true;
   }

   return {
      {"Input",  true},
      {"Output", true},
      {"Exc",    excHappened}
   };
}
