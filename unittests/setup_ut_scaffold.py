##
# @file    setup_ut_scaffold.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import argparse
import os
import sys

def gen_header_file(args: argparse.Namespace):
   """
   Generates header skeleton file.

   Args:
      args: Supplied options.
   """

   hdr_file = os.path.join(args.filepath, args.filename, "testsuite.h")

   if os.path.exists(hdr_file):
      if not args.hdr:
         return
 # else - continue...

   with open(hdr_file, "w") as outfile:
      def writeln(line):
         outfile.write(f"{line.rstrip()}\n")

      writeln(f"/**                                        ")
      writeln(f" * @file    testsuite.h                    ")
      writeln(f" * @version 1.0.0                          ")
      writeln(f" * @author  Forrest Jablonski              ")
      writeln(f" */                                        ")
      writeln(f"                                           ")
      writeln(f"#pragma once                               ")
      writeln(f"                                           ")
      writeln(f"#include \"utdefs.h\"                      ")
      writeln(f"#include \"testsuitebase.h\"               ")
      writeln(f"                                           ")
      writeln(f"namespace ym::ut                           ")
      writeln(f"{{                                         ")
      writeln(f"                                           ")
      writeln(f"/** TestSuite                              ")
      writeln(f" *                                         ")
      writeln(f" * @brief Test suite for {args.suitename}. ")
      writeln(f" */                                        ")
      writeln(f"class TestSuite : public TestSuiteBase     ")
      writeln(f"{{                                         ")
      writeln(f"public:                                    ")
      writeln(f"   explicit TestSuite(void);               ")
      writeln(f"   virtual ~TestSuite(void) = default;     ")
      writeln(f"                                           ")
      writeln(f"   YM_UT_TESTCASE(InteractiveInspection)   ")
      writeln(f"}};                                        ")
      writeln(f"                                           ")
      writeln(f"}} // ym::ut                               ") 

def gen_source_file(args: argparse.Namespace):
   """
   Generates source skeleton file.

   Args:
      args: Supplied options.
   """

   src_file = os.path.join(args.filepath, args.filename, "testsuite.cpp")

   if os.path.exists(src_file):
      if not args.src:
         return
 # else - continue...

   with open(src_file, "w") as outfile:
      def writeln(line):
         outfile.write(f"{line.rstrip()}\n")

      writeln(f"/**                                                                       ")
      writeln(f" * @file    testsuite.cpp                                                 ")
      writeln(f" * @version 1.0.0                                                         ")
      writeln(f" * @author  Forrest Jablonski                                             ")
      writeln(f" */                                                                       ")
      writeln(f"                                                                          ")
      writeln(f"#include \"ymglobals.h\"                                                  ")
      writeln(f"#include \"testsuite.h\"                                                  ")
      writeln(f"#include \"textlogger.h\"                                                 ")
      writeln(f"                                                                          ")
      writeln(f"#include \"{args.filename}.h\"                                            ")
      writeln(f"                                                                          ")
      writeln(f"/** TestSuite                                                             ")
      writeln(f" *                                                                        ")
      writeln(f" * @brief Constructor.                                                    ")
      writeln(f" */                                                                       ")
      writeln(f"ym::ut::TestSuite::TestSuite(void)                                        ")
      writeln(f"   : TestSuiteBase(\"{args.suitename}\")                                  ")
      writeln(f"{{                                                                        ")
      writeln(f"   addTestCase<InteractiveInspection>();                                  ")
      writeln(f"}}                                                                        ")
      writeln(f"                                                                          ")
      writeln(f"/** run                                                                   ")
      writeln(f" *                                                                        ")
      writeln(f" * @brief Interactive inspection - for debug purposes.                    ")
      writeln(f" *                                                                        ")
      writeln(f" * @returns DataShuttle -- Important values acquired during run of test.  ")
      writeln(f" */                                                                       ")
      writeln(f"auto ym::ut::TestSuite::InteractiveInspection" \
              f"::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle         ")
      writeln(f"{{                                                                        ")
      writeln(f"   auto const SE = ymLogPushEnable(VG::UnitTest_{args.suitename});        ")
      writeln(f"   return {{}};                                                           ")
      writeln(f"}}                                                                        ")

def gen_runner_file(args: argparse.Namespace):
   """
   Generates test suite runner skeleton file.

   Args:
      args: Supplied options.
   """

   runner_file = os.path.join(args.filepath, args.filename, "testsuite.py")

   if os.path.exists(runner_file):
      if not args.run:
         return
 # else - continue...

   with open(runner_file, "w") as outfile:
      def writeln(line):
         outfile.write(f"{line.rstrip()}\n")
   
      writeln(f"##                                                                  ")
      writeln(f"# @file    testsuite.py                                             ")
      writeln(f"# @version 1.0.0                                                    ")
      writeln(f"# @author  Forrest Jablonski                                        ")
      writeln(f"#                                                                   ")
      writeln(f"                                                                    ")
      writeln(f"import os                                                           ")
      writeln(f"import sys                                                          ")
      writeln(f"import unittest                                                     ")
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
      writeln(f"      super().setUpBaseClass(                                       ")
      writeln(f"         filepath=\"{args.filepath}\",                              ")
      writeln(f"         filename=\"{args.filename}\")                              ")
      writeln(f"                                                                    ")
      writeln(f"   @classmethod                                                     ")
      writeln(f"   def tearDownClass(cls):                                          ")
      writeln(f"      \"\"\"                                                        ")
      #TODO
      writeln(f"      Acting destructor.                                            ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      super().tearDownBaseClass()                                   ")
      writeln(f"                                                                    ")
      writeln(f"   def setUp(self):                                                 ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      @brief Set up logic that is run before each test.             ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      pass                                                          ")
      writeln(f"                                                                    ")
      writeln(f"   def tearDown(self):                                              ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      @brief Tear down logic that is run after each test.           ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      pass                                                          ")
      writeln(f"                                                                    ")
      writeln(f"   def test_InteractiveInspection(self):                            ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      @brief Analyzes results from test case.                       ")
      writeln(f"                                                                    ")
      writeln(f"      @param results -- Results from test case.                     ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"                                                                    ")
      writeln(f"      from cppyy.gbl import std                                     ")
      writeln(f"      from cppyy.gbl import ym                                      ")
      writeln(f"                                                                    ")
      writeln(f"      # uncomment to run test                                       ")
      writeln(f"      # results = self.run_test_case(\"InteractiveInspection\")     ")
      writeln(f"                                                                    ")
      writeln(f"# kick-off                                                          ")
      writeln(f"if __name__ == \"__main__\":                                        ")
      writeln(f"   if os.path.basename(os.getcwd()) != \"{args.filename}\":         ")
      writeln(f"      print(\"Needs to be run in the {args.filename}/ directory\")  ")
      writeln(f"      sys.exit(1)                                                   ")
      writeln(f"                                                                    ")
      writeln(f"   unittest.main()                                                  ")

def main():
   """
   Sets up the unittest scaffolding.

   Notes:
      For example...
         --filepath  ym/common/
         --filename  ymdefs (w/out extension)
         --suitename YmDefs
   """
   if os.path.basename(os.getcwd()) != "unittests":
      print("Needs to be run in the unittests/ directory")
      sys.exit(1)

   parser = argparse.ArgumentParser()
   parser.add_argument("--filepath",    required=True, help="Relative path file exists in")
   parser.add_argument("--filename",    required=True, help="Name of file (w/out extension)")
   parser.add_argument("--suitename",   required=True, help="Structure(s) under test")
   parser.add_argument("--hdr", action="store_true",   help="Generates/overwrites header")
   parser.add_argument("--src", action="store_true",   help="Generates/overwrites source")
   parser.add_argument("--run", action="store_true",   help="Generates/overwrites runner")
   args = parser.parse_args()

   os.makedirs(os.path.join(args.filepath, args.filename), exist_ok=True)

   gen_header_file(args)
   gen_source_file(args)
   gen_runner_file(args)

# kick-off
if __name__ == "__main__":
   main()
else:
   print("Meant to run stand-alone - not to be imported.")
   sys.exit(1)
