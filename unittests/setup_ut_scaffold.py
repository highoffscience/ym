##
# @file    setup_ut_scaffold.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import argparse
import os
import sys

def gen_hdr_file(args):
   """
   @brief Generates header skeleton file.

   @param args -- Supplied options.
   """

   hdr_file = os.path.join(args.filepath, args.filename, "testsuite.h")

   if os.path.exists(hdr_file):
      return
 # else - continue...

   with open(hdr_file, "w") as outfile:
      def writeln(line):
         outfile.write(f"{line.rstrip()}\n")

      writeln(f"/**                                           ")
      writeln(f" * @file    testsuite.h                       ")
      writeln(f" * @version 1.0.0                             ")
      writeln(f" * @author  Forrest Jablonski                 ")
      writeln(f" */                                           ")
      writeln(f"                                              ")
      writeln(f"#pragma once                                  ")
      writeln(f"                                              ")
      writeln(f"#include \"ut.h\"                             ")
      writeln(f"                                              ")
      writeln(f"#include \"testsuitebase.h\"                  ")
      writeln(f"                                              ")
      writeln(f"namespace ym::ut                              ")
      writeln(f"{{                                            ")
      writeln(f"                                              ")
      writeln(f"/** TestSuite                                 ")
      writeln(f" *                                            ")
      writeln(f" * @brief Test suite for SUT {args.SUT_name}. ")
      writeln(f" */                                           ")
      writeln(f"class TestSuite : public TestSuiteBase        ")
      writeln(f"{{                                            ")
      writeln(f"public:                                       ")
      writeln(f"   explicit TestSuite(void);                  ")
      writeln(f"   virtual ~TestSuite(void) = default;        ")
      writeln(f"                                              ")
      writeln(f"   // YM_UT_TESTCASE(name_of_test_case_here)  ")
      writeln(f"}};                                           ")
      writeln(f"                                              ")
      writeln(f"}} // ym::ut                                  ") 

def gen_src_file(args):
   """
   @brief Generates source skeleton file.

   @param args -- Supplied options.
   """

   src_file = os.path.join(args.filepath, args.filename, "testsuite.cpp")

   if os.path.exists(src_file):
      return
 # else - continue...

   with open(src_file, "w") as outfile:
      def writeln(line):
         outfile.write(f"{line.rstrip()}\n")

      writeln(f"/**                                                                     ")
      writeln(f" * @file    testsuite.cpp                                               ")
      writeln(f" * @version 1.0.0                                                       ")
      writeln(f" * @author  Forrest Jablonski                                           ")
      writeln(f" */                                                                     ")
      writeln(f"                                                                        ")
      writeln(f"#include \"ym.h\"                                                       ")
      writeln(f"#include \"testsuite.h\"                                                ")
      writeln(f"                                                                        ")
      writeln(f"#include \"{args.filename}.h\"                                          ")
      writeln(f"                                                                        ")
      writeln(f"/** TestSuite                                                           ")
      writeln(f"*                                                                       ")
      writeln(f"* @brief Constructor.                                                   ")
      writeln(f"*/                                                                      ")
      writeln(f"ym::ut::TestSuite::TestSuite(void)                                      ")
      writeln(f"   : TestSuiteBase(\"{args.SUT_name}\")                                 ")
      writeln(f"{{                                                                      ")
      writeln(f"   addTestCase<name_of_test_case_here>();                               ")
      writeln(f"}}                                                                      ")
      writeln(f"                                                                        ")
      writeln(f"/** run                                                                 ")
      writeln(f" *                                                                      ")
      writeln(f" * @brief TODO.                                                         ")
      writeln(f" *                                                                      ")
      writeln(f" * @return DataShuttle -- Important values acquired during run of test. ")
      writeln(f" */                                                                     ")
      writeln(f"auto ym::ut::TestSuite::name_of_test_case_here" \
              f"::run([[maybe_unused]] DataShuttle const & InData) -> DataShuttle       ")
      writeln(f"{{                                                                      ")
      writeln(f"   return {{}};                                                         ")
      writeln(f"}}                                                                      ")

def gen_runner_file(args):
   """ gen_runner_file
   @brief Generates test suite runner skeleton file.

   @param args -- Supplied options.
   """

   runner_file = os.path.join(args.filepath, args.filename, "testsuite.py")

   if os.path.exists(runner_file):
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
      writeln(f"                                                                    ")
      writeln(f"try:                                                                ")
      writeln(f"   # @note Grabs the first directory in the chain named unittests/. ")
      writeln(f"   sys.path.append(os.path.join(" \
              f"os.getcwd().split(\"unittests\")[0], \"unittests/\"))               ")
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
      writeln(f"   @brief Collection of all tests for SUT {args.SUT_name}.          ")
      writeln(f"   \"\"\"                                                           ")
      writeln(f"                                                                    ")
      writeln(f"   def __init__(self):                                              ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      @brief Constructor.                                           ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      super().__init__(\"{args.filepath}\", " \
                                     f"\"{args.filename}\", " \
                                     f"\"{args.SUT_name}\", [])                     ")
      writeln(f"                                                                    ")
      writeln(f"      self.test_cases = {{                                          ")
      writeln(f"         \"name_of_test_case_here\": "
              f"self.analyze_results_name_of_test_case_here                         ")
      writeln(f"      }}                                                            ")
      writeln(f"                                                                    ")
      writeln(f"   def run(self, test_case_names:list=None):                        ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      @brief Runs the test suite.                                   ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"                                                                    ")
      writeln(f"      from cppyy.gbl import std                                     ")
      writeln(f"      from cppyy.gbl import ym                                      ")
      writeln(f"                                                                    ")
      writeln(f"      ts = ym.ut.TestSuite()                                        ")
      writeln(f"                                                                    ")
      writeln(f"      if test_case_names is not None:                               ")
      writeln(f"         for name in test_case_names:                               ")
      writeln(f"            try:                                                    ")
      writeln(f"               results = ts.runTestCase(name)                       ")
      writeln(f"               self.test_cases[name](results)                       ")
      writeln(f"            except:                                                 ")
      writeln(f"               print(f\"Something failed with test case {{name}}\") ")
      writeln(f"      else:                                                         ")
      writeln(f"         all_results = ts.runAllTestCases()                         ")
      writeln(f"         for test_case_data in all_results:                         ")
      writeln(f"            name = test_case_data.first                             ")
      writeln(f"            test_case_results = test_case_data.second               ")
      writeln(f"            self.test_cases[name](test_case_results)                ")
      writeln(f"                                                                    ")
      writeln(f"   def analyze_results_name_of_test_case_here(self, results):       ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"      @brief Analyzes results from test case.                       ")
      writeln(f"                                                                    ")
      writeln(f"      @param results -- Results from test case.                     ")
      writeln(f"      \"\"\"                                                        ")
      writeln(f"                                                                    ")
      writeln(f"      from cppyy.gbl import std                                     ")
      writeln(f"      from cppyy.gbl import ym                                      ")
      writeln(f"                                                                    ")
      writeln(f"      pass                                                          ")
      writeln(f"                                                                    ")
      writeln(f"# kick-off                                                          ")
      writeln(f"if __name__ == \"__main__\":                                        ")
      writeln(f"   if os.path.basename(os.getcwd()) != \"{args.filename}\":         ")
      writeln(f"      print(\"Needs to be run in the {args.filename}/ directory\")  ")
      writeln(f"      sys.exit(1)                                                   ")
      writeln(f"                                                                    ")
      writeln(f"   ts = TestSuite()                                                 ")
      writeln(f"   ts.run()                                                         ")

""" main

@brief Sets up the unittest scaffolding.

@note For example...
      --filepath ym/common/
      --filename filename
      --SUT_name SUTName
"""
def main():
   if os.path.basename(os.getcwd()) != "unittests":
      print("Needs to be run in the unittests/ directory")
      sys.exit(1)

   parser = argparse.ArgumentParser()
   parser.add_argument("--filepath", required=True, help="relative path file exists in", type=str)
   parser.add_argument("--filename", required=True, help="name of file",                 type=str)
   parser.add_argument("--SUT_name", required=True, help="name of Structure Under Test", type=str)
   args = parser.parse_args()

   os.makedirs(os.path.join(args.filepath, args.filename), exist_ok=True)

   gen_hdr_file   (args)
   gen_src_file   (args)
   gen_runner_file(args)

# kick-off
if __name__ == "__main__":
   main()
else:
   print("Meant to run stand-alone - not to be imported.")
   sys.exit(1)
