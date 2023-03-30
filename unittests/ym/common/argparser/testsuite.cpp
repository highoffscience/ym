/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"
#include "testsuite.h"

#include "argparser.h"
#include "textlogger.h"

#include <cstring>

#include <boost/stacktrace.hpp>

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
   TextLogger::getGlobalInstance()->enable(VG::ArgParser);
   TextLogger::getGlobalInstance()->enable(VG::General  );
   TextLogger::getGlobalInstance()->enable(VG::Ymception);

   str  const Argv[] = {"testsuite", "--input", "settings.json", "--output", "data.csv"};
   auto const Argc   = static_cast<int32>(YM_ARRAY_SIZE(Argv));
   
   auto excHappened = false;

   ymLog(VG::General, "stack depth %u", boost::stacktrace::basic_stacktrace().size());

   ArgParser argparse;
   try
   {
      argparse.parse({
         argparse.arg("input").desc("input file"),
         argparse.arg("output").desc("output file")
      },
      Argc, Argv);
   }
   catch (ArgParser::ArgParserError const & E)
   {
      excHappened = true;
   }

   auto const * InputArg_Ptr  = (excHappened) ? nullptr : argparse["input" ];
   auto const * OutputArg_Ptr = (excHappened) ? nullptr : argparse["output"];

   auto const * InputVal_Ptr  = (InputArg_Ptr ) ? InputArg_Ptr ->getVal() : nullptr;
   auto const * OutputVal_Ptr = (OutputArg_Ptr) ? OutputArg_Ptr->getVal() : nullptr;

   auto const Input  = (InputVal_Ptr)  ? (std::strcmp(InputVal_Ptr,  "settings.json") == 0_i32) : false;
   auto const Output = (OutputVal_Ptr) ? (std::strcmp(OutputVal_Ptr, "data.csv"     ) == 0_i32) : false;

   return {
      {"Input",  Input},
      {"Output", Output},
      {"Exc",    excHappened}
   };
}
