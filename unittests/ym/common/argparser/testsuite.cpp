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
   auto const Argc = static_cast<int32>(YM_ARRAY_SIZE(Argv));
   
   auto excHappened = false;
   auto val_input   = false;
   auto val_output  = false;
   auto val_clean   = false;

   auto & ap_ref = *ArgParser::getInstancePtr();
   try
   {
      ap_ref.parse({
         ap_ref.arg("input" ).desc("Input file"  ),
         ap_ref.arg("output").desc("Output file" ),
         ap_ref.arg("clean" ).desc("Cleans build").flag()
      },
      Argc, Argv);

      val_input  = std::strcmp(ap_ref["input" ]->getVal(), "settings.json") == 0_i32;
      val_output = std::strcmp(ap_ref["output"]->getVal(), "data.csv"     ) == 0_i32;
      val_clean  =             ap_ref["clean" ]->isEnbl();
   }
   catch (ArgParser::ArgParserError const & E)
   {
      excHappened = true;
   }

   return {
      {"Exc",    excHappened},
      {"Input",  val_input  },
      {"Output", val_output },
      {"Clean",  val_clean  }
   };
}
