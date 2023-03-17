/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ym.h"
#include "testsuite.h"

#include "argparser.h"
#include "textlogger.h"

#include <cstring>

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
   TextLogger::getGlobalInstance()->enable(VGM_T::ArgParser);
   TextLogger::getGlobalInstance()->enable(VGM_T::General  );
   TextLogger::getGlobalInstance()->enable(VGM_T::Ymception);

   str  const Argv[] = {"testsuite", "--input", "settings.json", "--output", "data.csv"};
   auto const Argc   = static_cast<int32>(YM_ARRAY_SIZE(Argv));
   
   auto excHappened = false;

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

   auto const Input  = (std::strcmp(argparse["input" ]->getVal(), "settings.json") == 0_i32);
   auto const Output = (std::strcmp(argparse["output"]->getVal(), "data.csv"     ) == 0_i32);

   return {
      {"Input",  Input},
      {"Output", Output},
      {"Exc",    excHappened}
   };
}
