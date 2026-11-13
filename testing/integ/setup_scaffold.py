##
# @file    setup_scaffold.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import argparse
import os
import pathlib
import sys

def gen_header_file(args: argparse.Namespace):
   """
   Generates header skeleton file.

   Args:
      args: Supplied options.
   """

   hdr_file: pathlib.Path = args.filepath / "testsuite.h"

   if hdr_file.exists():
      if not args.hdr:
         return
 # else - continue...

   with open(hdr_file, "w") as outfile:
      def writeln(line):
         outfile.write(f"{line.rstrip()}\n")

      namespace = f"ym::integ::{args.filename.as_posix().replace('/', '::')}"

      writeln(f"/**                                         ")
      writeln(f" * @file    testsuite.h                     ")
      writeln(f" * @version {args.version}                  ")
      writeln(f" * @author  {args.author}                   ")
      writeln(f" */                                         ")
      writeln(f"                                            ")
      writeln(f"#pragma once                                ")
      writeln(f"                                            ")
      writeln(f"#include \"testsuitebase.h\"                ")
      writeln(f"                                            ")
      writeln(f"namespace {namespace}                       ")
      writeln(f"{{                                          ")
      writeln(f"                                            ")
      writeln(f"/**                                         ")
      writeln(f" * @brief Test suite for {args.suitename}.  ")
      writeln(f" */                                         ")
      writeln(f"class TestSuite : public TestSuiteBase      ")
      writeln(f"{{                                          ")
      writeln(f"public:                                     ")
      writeln(f"   explicit TestSuite(void);                ")
      writeln(f"   virtual ~TestSuite(void) = default;      ")
      writeln(f"                                            ")
      writeln(f"   YM_INTEG_TESTCASE(InteractiveInspection) ")
      writeln(f"   YM_INTEG_TESTCASE(SmokeTest)             ")
      writeln(f"}};                                         ")
      writeln(f"                                            ")
      writeln(f"}} // {namespace}                           ")

def gen_source_file(args: argparse.Namespace):
   """
   Generates source skeleton file.

   Args:
      args: Supplied options.
   """

   src_file: pathlib.Path = args.filepath / "testsuite.cpp"

   if src_file.exists():
      if not args.src:
         return
 # else - continue...

   with open(src_file, "w") as outfile:
      def writeln(line):
         outfile.write(f"{line.rstrip()}\n")

      namespace = f"ym::integ::{args.filename.as_posix().replace('/', '::')}"

      writeln(f"/**                                                               ")
      writeln(f" * @file    testsuite.cpp                                         ")
      writeln(f" * @version {args.version}                                        ")
      writeln(f" * @author  {args.author}                                         ")
      writeln(f" */                                                               ")
      writeln(f"                                                                  ")
      writeln(f"#define YM_INTEG_TESTING_ACTIVE_DEFINED                           ")
      writeln(f"                                                                  ")
      writeln(f"#include \"testsuite.h\"                                          ")
      writeln(f"#include \"ymglobals.h\"                                          ")
      writeln(f"                                                                  ")
      writeln(f"// Begin - Structure(s) under test                                ")
      writeln(f"// List includes here...                                          ")
      writeln(f"// End - Structure(s) under test                                  ")
      writeln(f"                                                                  ")
      writeln(f"#include \"fmt/format.h\"                                         ")
      writeln(f"                                                                  ")
      writeln(f"/**                                                               ")
      writeln(f" * @brief Constructor.                                            ")
      writeln(f" */                                                               ")
      writeln(f"{namespace}::TestSuite::TestSuite(void) :                         ")
      writeln(f"   TestSuiteBase(\"{args.suitename}\")                            ")
      writeln(f"{{                                                                ")
      writeln(f"   addTestCase<InteractiveInspection>();                          ")
      writeln(f"   addTestCase<SmokeTest>();                                      ")
      writeln(f"}}                                                                ")
      writeln(f"                                                                  ")
      writeln(f"/**                                                               ")
      writeln(f" * @brief Interactive inspection - for debug purposes.            ")
      writeln(f" *                                                                ")
      writeln(f" * @param InData -- Important values for test.                    ")
      writeln(f" *                                                                ")
      writeln(f" * @returns DataShuttle -- Important values acquired during test. ")
      writeln(f" */                                                               ")
      writeln(f"auto {namespace}::TestSuite::InteractiveInspection" \
              f"::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle ")
      writeln(f"{{                                                                ")
      writeln(f"   return {{}};                                                   ")
      writeln(f"}}                                                                ")
      writeln(f"                                                                  ")
      writeln(f"/**                                                               ")
      writeln(f" * @brief Basic integrity test.                                   ")
      writeln(f" *                                                                ")
      writeln(f" * @param InData -- Important values for test.                    ")
      writeln(f" *                                                                ")
      writeln(f" * @returns DataShuttle -- Important values acquired during test. ")
      writeln(f" */                                                               ")
      writeln(f"auto {namespace}::TestSuite::SmokeTest" \
              f"::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle ")
      writeln(f"{{                                                                ")
      writeln(f"   return {{}};                                                   ")
      writeln(f"}}                                                                ")

def gen_runner_file(args: argparse.Namespace):
   """
   Generates test suite runner skeleton file.

   Args:
      args: Supplied options.
   """

   runner_file: pathlib.Path = args.filepath / "testsuite.py"

   if runner_file.exists():
      if not args.run:
         return
 # else - continue...

   with open(runner_file, "w") as outfile:
      def writeln(line):
         outfile.write(f"{line.rstrip()}\n")

      writeln(f"##                                                                  ")
      writeln(f"# @file    testsuite.py                                             ")
      writeln(f"# @version {args.version}                                           ")
      writeln(f"# @author  {args.author}                                            ")
      writeln(f"#                                                                   ")
      writeln(f"                                                                    ")
      writeln(f"import sys                                                          ")
      writeln(f"                                                                    ")
      writeln(f"try:                                                                ")
      writeln(f"   import testsuitebase                                             ")
      writeln(f"except:                                                             ")
      writeln(f"   print(\"Cannot import testsuitebase - path set correctly?\")     ")
      writeln(f"   sys.exit(1)                                                      ")
      writeln(f"                                                                    ")
      writeln(f"try:                                                                ")
      writeln(f"   import cppyy                                                     ")
      writeln(f"except:                                                             ")
      writeln(f"   print(\"Cannot import cppyy - started the venv?\")               ")
      writeln(f"   sys.exit(1)                                                      ")
      writeln(f"                                                                    ")
      writeln(f"class TestSuite(testsuitebase.TestSuiteBase):                       ")
      writeln(f"   \"\"\"                                                           ")
      writeln(f"   Collection of all tests for suite {args.suitename}.              ")
      writeln(f"   \"\"\"                                                           ")
      writeln(f"                                                                    ")
      writeln(f"   @classmethod                                                     ")
      writeln(f"   def setUpClass(cls):                                             ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      Acting constructor.                                           ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      super().setUpBaseClass(filepath=\"{args.filepath}\"           ")
      writeln(f"                                                                    ")
      writeln(f"   @classmethod                                                     ")
      writeln(f"   def tearDownClass(cls):                                          ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      Acting destructor.                                            ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      super().tearDownBaseClass()                                   ")
      writeln(f"                                                                    ")
      writeln(f"   def setUp(self):                                                 ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      Set up logic that is run before each test.                    ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      pass                                                          ")
      writeln(f"                                                                    ")
      writeln(f"   def tearDown(self):                                              ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      Tear down logic that is run after each test.                  ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      pass                                                          ")
      writeln(f"                                                                    ")
      writeln(f"   def test_InteractiveInspection(self):                            ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      Analyzes results from test case.                              ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      from cppyy.gbl import std # type: ignore                      ")
      writeln(f"      from cppyy.gbl import ym  # type: ignore                      ")
      writeln(f"                                                                    ")
      writeln(f"      # uncomment to run test                                       ")
      writeln(f"      # results = self.run_test_case(" \
              f"self._testMethodName.removeprefix(\"test_\"), assert_results=False) ")
      writeln(f"      pass                                                          ")
      writeln(f"                                                                    ")
      writeln(f"   def test_SmokeTest(self):                                        ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      Analyzes results from test case.                              ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      from cppyy.gbl import std # type: ignore                      ")
      writeln(f"      from cppyy.gbl import ym  # type: ignore                      ")
      writeln(f"                                                                    ")
      writeln(f"      # uncomment to run test                                       ")
      writeln(f"      # results = self.run_test_case(" \
              f"self._testMethodName.removeprefix(\"test_\"), assert_results=False) ")
      writeln(f"      pass                                                          ")
      writeln(f"                                                                    ")
      writeln(f"# kick-off                                                          ")
      writeln(f"if __name__ == \"__main__\":                                        ")
      writeln(f"   TestSuite.runSuite()                                             ")
      writeln(f"else:                                                               ")
      writeln(f"   TestSuite.runSuite()                                             ")

def main():
   """
   Sets up the testing scaffolding.

   Notes:
      For example...
         --filepath  ym/common/
         --suitename YmDefs
   """

   if os.path.basename(os.getcwd()) != "integ":
      print("Needs to be run in testing/integ/ directory")
      sys.exit(1)

   parser = argparse.ArgumentParser()
   parser.add_argument("--filepath",  required=True,   help="Relative path suite exists in", type=pathlib.Path)
   parser.add_argument("--suitename", required=True,   help="Name of testing suite",         type=pathlib.Path)
   parser.add_argument("--author",    required=True,   help="Name of current author")
   parser.add_argument("--version",   default="1.0.0", help="Current version")
   parser.add_argument("--hdr", action="store_true",   help="Generates/overwrites header")
   parser.add_argument("--src", action="store_true",   help="Generates/overwrites source")
   parser.add_argument("--run", action="store_true",   help="Generates/overwrites runner")
   args = parser.parse_args()

   os.makedirs(args.filepath, exist_ok=True)

   gen_header_file(args)
   gen_source_file(args)
   gen_runner_file(args)

# kick-off
if __name__ == "__main__":
   main()
else:
   print("Meant to run stand-alone - not to be imported.")
   sys.exit(1)
