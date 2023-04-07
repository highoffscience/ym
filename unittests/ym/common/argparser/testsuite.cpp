/**
 * @file    testsuite.cpp
 * @version 1.0.0
 * @author  Forrest Jablonski
 */

#include "ymdefs.h"
#include "testsuite.h"

#include "argparser.h"
#include "ops.h"
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
   TextLogger::getGlobalInstance()->enable(VG::ArgParser);
   TextLogger::getGlobalInstance()->enable(VG::UnitTest_ArgParser);
   TextLogger::getGlobalInstance()->enable(VG::Ymception);

   str  const Argv[] = {"testsuite",
      "--input",  "settings.json",
      "--output", "data.csv",
      "--clean"
   };
   auto const Argc   = static_cast<int32>(YM_ARRAY_SIZE(Argv));
   
   auto excHappened = false;
   auto val_input   = "";
   auto val_output  = "";

   ArgParser argparse;
   try
   {
      argparse.parse({
         argparse.arg("input").desc("Input file"),
         argparse.arg("output").desc("Output file"),
         argparse.arg("clean").desc("Cleans the build").flag()
      },
      Argc, Argv);

      val_input  = argparse["input" ]->getVal();
      val_output = argparse["output"]->getVal();
   }
   catch (ArgParser::ArgParserError const & E)
   {
      excHappened = true;
   }

   auto const Input  = std::strcmp(val_input,  "settings.json") == 0_i32;
   auto const Output = std::strcmp(val_output, "data.csv"     ) == 0_i32;

   return {
      {"Input",  Input},
      {"Output", Output},
      {"Exc",    excHappened}
   };
}
