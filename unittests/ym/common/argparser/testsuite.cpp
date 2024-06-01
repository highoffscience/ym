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

#include <array>
#include <cstring>

/** TestSuite
 *
 * @brief Constructor.
 */
ym::ut::TestSuite::TestSuite(void)
   : TestSuiteBase("ArgParser")
{
   addTestCase<BasicParse   >();
   addTestCase<FlagIntegrity>();
}

/** run
 *
 * @brief Tests if ArgParser can parse.
 * 
 * @throws TODO
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::BasicParse::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_ArgParser);

   str const Argv[] = {"testsuite",
      "--input",  "settings.json",
      "--output", "data.csv",
      "-cb",
      "-k", "Torchic1234",
      "--in-denial"
   };
   auto const Argc = static_cast<int32>(std::size(Argv));

   std::array argHandlers{
      ArgParser::Arg("input"    ).desc("Input file"  ),
      ArgParser::Arg("output"   ).desc("Output file" ),
      ArgParser::Arg("clean"    ).desc("Cleans build").abbr('c').enbl(),
      ArgParser::Arg("build"    ).desc("Builds exe"  ).abbr('b').enbl(),
      ArgParser::Arg("key"      ).desc("Passkey"     ).abbr('k'),      
      ArgParser::Arg("in-denial").desc("My existence")          .enbl()
   };
   
   ArgParser ap(Argc, Argv, argHandlers);

   auto excHappened = false;
   auto val_input   = false;
   auto val_output  = false;
   auto val_clean   = false;
   auto val_build   = false;
   auto val_key     = false;
   auto val_denial  = false;

   try
   {
      ap.parse();

      val_input  = std::strcmp(ap["input"    ]->getVal(), "settings.json") == 0_i32;
      val_output = std::strcmp(ap["output"   ]->getVal(), "data.csv"     ) == 0_i32;
      val_clean  =             ap["clean"    ]->isEnbl();
      val_build  =             ap["build"    ]->isEnbl();
      val_key    = std::strcmp(ap["key"      ]->getVal(), "Torchic1234"  ) == 0_i32;
      val_denial =             ap["in-denial"]->isEnbl();
   }
   catch (ArgParser::ArgParserError const & E)
   {
      ymLog(VG::UnitTest_ArgParser, "--> %s", E.what());
      excHappened = true;
   }

   return {
      {"Exc",    excHappened},
      {"Input",  val_input  },
      {"Output", val_output },
      {"Clean",  val_clean  },
      {"Build",  val_build  },
      {"Key",    val_key    },
      {"Denial", val_denial }
   };
}

/** run
 *
 * @brief Tests if ArgParser can parse.
 * 
 * @throws TODO
 *
 * @returns DataShuttle -- Important values acquired during run of test.
 */
auto ym::ut::TestSuite::FlagIntegrity::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle
{
   auto const SE = ymLogPushEnable(VG::UnitTest_ArgParser);

   str const Argv[] = {"testsuite",
      "--verbose",
      "--width", "1"
   };
   auto const Argc = static_cast<int32>(std::size(Argv));

   std::array argHandlers{
      ArgParser::Arg("width"  ).desc("Width"    ),
      ArgParser::Arg("verbose").desc("Verbosity").enbl()
   };
   
   ArgParser ap(Argc, Argv, argHandlers);
   
   auto excHappened = false;
   auto val_verbose = false;
   auto val_width   = false;

   try
   {
      ap.parse();

      val_verbose = ap["verbose"]->isFlag();
      val_width   = ap["width"  ]->isFlag() == false;
   }
   catch (ArgParser::ArgParserError const & E)
   {
      ymLog(VG::UnitTest_ArgParser, "--> %s", E.what());
      excHappened = true;
   }

   return {
      {"Exc",     excHappened},
      {"Verbose", val_verbose},
      {"Width",   val_width  }
   };
}
