##
# @file    setup_ut_scaffold.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import argparse
import os
import sys

""" gen_cmake_file

@brief Generates cmake skeleton file.
"""
def gen_cmake_file(args):
   cmake_file = os.path.join(args.filepath, args.filename, "CMakeLists.txt")
   if not os.path.exists(cmake_file):
      with open(cmake_file, "w") as outfile:
         def writeln(line):
            outfile.write(f"{line.rstrip()}\n")

         writeln(f"##                                                                             ")
         writeln(f"# @file    CMakeLists.txt                                                      ")
         writeln(f"# @version 1.0.0                                                               ")
         writeln(f"# @author  Forrest Jablonski                                                   ")
         writeln(f"#                                                                              ")
         writeln(f"                                                                               ")
         writeln(f"cmake_minimum_required(VERSION 3.16)                                           ")
         writeln(f"                                                                               ")
         writeln(f"project({args.filename} VERSION 1.0.0 DESCRIPTION " \
                 f"\"Builds {args.SUT_name} shared libraries\")                                   ")
         writeln(f"                                                                               ")
         writeln(f"set(CMAKE_CXX_STANDARD 20)                                                     ")
         writeln(f"set(CMAKE_CXX_STANDARD_REQUIRED True)                                          ")
         writeln(f"                                                                               ")
         writeln(f"set(RootDir        ${{CMAKE_CURRENT_SOURCE_DIR}}/../../)                       ")
         writeln(f"set(CommonDir      ${{RootDir}}/ym/common/)                                    ")
         writeln(f"set(UnitTestDir    ${{RootDir}}/ym/unittests/)                                 ")
         writeln(f"set(SUTDir         ${{RootDir}}/{args.filepath}/)                              ")
         writeln(f"set(SUTUnitTestDir ${{UnitTestDir}}/{args.filepath}/{args.filename}/)          ")
         writeln(f"                                                                               ")
         writeln(f"add_library({args.filename} SHARED)                                            ")
         writeln(f"                                                                               ")
         writeln(f"target_sources({args.filename} PRIVATE                                         ")
         writeln(f"   ${{SUTDir}}/{args.filename}.cpp                                             ")
         writeln(f"   ${{UnitTestDir}}/common/testcase.cpp                                        ")
         writeln(f"   ${{UnitTestDir}}/common/datashuttle.cpp                                     ")
         writeln(f"   ${{UnitTestDir}}/common/unittestbase.cpp                                    ")
         writeln(f"   ${{UnitTestDir}}/common/nameable.cpp                                        ")
         writeln(f"   ${{SUTUnitTestDir}}/testsuite.cpp)                                          ")
         writeln(f"                                                                               ")
         writeln(f"set_target_properties({args.filename} PROPERTIES VERSION ${{PROJECT_VERSION}}) ")
         writeln(f"set_target_properties({args.filename} PROPERTIES LIBRARY_OUTPUT_DIRECTORY " \
                 f"${{CMAKE_BINARY_DIR}}/{args.filepath})                                         ")
         writeln(f"                                                                               ")
         writeln(f"target_compile_options({args.filename} PRIVATE -Werror -Wall -Wextra -O2)      ")
         writeln(f"                                                                               ")
         writeln(f"target_include_directories({args.filename} PRIVATE ${{CommonDir}})             ")
         writeln(f"target_include_directories({args.filename} PRIVATE ${{UnitTestDir}})           ")
         writeln(f"target_include_directories({args.filename} PRIVATE ${{SUTDir}})                ")
         writeln(f"target_include_directories({args.filename} PRIVATE ${{SUTUnitTestDir}})        ")

""" gen_hdr_file

@brief Generates header skeleton file.
"""
def gen_hdr_file(args):
   hdr_file = os.path.join(args.filepath, args.filename, "testsuite.h")
   if not os.path.exists(hdr_file):
      with open(hdr_file, "w") as outfile:
         def writeln(line):
            outfile.write(f"{line.rstrip()}\n")

         writeln(f"/**                                                ")
         writeln(f" * @file    testsuite.h                            ")
         writeln(f" * @version 1.0.0                                  ")
         writeln(f" * @author  Forrest Jablonski                      ")
         writeln(f" */                                                ")
         writeln(f"                                                   ")
         writeln(f"#pragma once                                       ")
         writeln(f"                                                   ")
         writeln(f"#include \"ut.h\"                                  ")
         writeln(f"                                                   ")
         writeln(f"#include \"testsuite.h\"                           ")
         writeln(f"                                                   ")
         writeln(f"namespace ym::ut                                   ")
         writeln(f"{{                                                 ")
         writeln(f"                                                   ")
         writeln(f"/** TestSuite                                      ")
         writeln(f"*                                                  ")
         writeln(f"* @brief Test suite for structure {args.SUT_name}. ")
         writeln(f"*/                                                 ")
         writeln(f"class TestSuite : public TestSuiteBase             ")
         writeln(f"{{                                                 ")
         writeln(f"public:                                            ")
         writeln(f"   explicit TestSuite(void);                       ")
         writeln(f"   virtual ~TestSuite(void) = default;             ")
         writeln(f"                                                   ")
         writeln(f"   // YM_UT_TESTCASE(name_of_test_case_here);      ")
         writeln(f"}};                                                ")
         writeln(f"                                                   ")
         writeln(f"}} // ym::ut                                       ") 

""" gen_src_file

@brief Generates source skeleton file.
"""
def gen_src_file(args):
   src_file = os.path.join(args.filepath, args.filename, "testsuite.cpp")
   if not os.path.exists(src_file):
      with open(src_file, "w") as outfile:
         def writeln(line):
            outfile.write(f"{line.rstrip()}\n")

         writeln(f"/**                                                                                            ")
         writeln(f" * @file    testsuite.cpp                                                                      ")
         writeln(f" * @version 1.0.0                                                                              ")
         writeln(f" * @author  Forrest Jablonski                                                                  ")
         writeln(f" */                                                                                            ")
         writeln(f"                                                                                               ")
         writeln(f"#include \"ut.h\"                                                                              ")
         writeln(f"#include \"testsuite.h\"                                                                       ")
         writeln(f"                                                                                               ")
         writeln(f"#include \"{args.filename}.h\"                                                                 ")
         writeln(f"                                                                                               ")
         writeln(f"/** TestSuite                                                                                  ")
         writeln(f"*                                                                                              ")
         writeln(f"* @brief Constructor.                                                                          ")
         writeln(f"*/                                                                                             ")
         writeln(f"ym::ut::TestSuite::TestSuite(void)                                                             ")
         writeln(f"   : TestSuiteBase(\"{args.SUT_name}\")                                                        ")
         writeln(f"{{                                                                                             ")
         writeln(f"   addTestCase<name_of_test_case_here>();                                                      ")
         writeln(f"}}                                                                                             ")
         writeln(f"                                                                                               ")
         writeln(f"/** run                                                                                        ")
         writeln(f" *                                                                                             ")
         writeln(f" * @brief TODO.                                                                                ")
         writeln(f" *                                                                                             ")
         writeln(f" * @return DataShuttle -- Important values acquired during run of test case.                   ")
         writeln(f" */                                                                                            ")
         writeln(f"auto ym::ut::TestSuite::name_of_test_case_here::run(DataShuttle const & InData) -> DataShuttle ")
         writeln(f"{{                                                                                             ")
         writeln(f"   return {{}};                                                                                ")
         writeln(f"}}                                                                                             ")

""" gen_runner_file

Generates test suite runner skeleton file.
"""
def gen_runner_file(args):
   runner_file = os.path.join(args.filepath, args.filename, "testsuite.py")
   if not os.path.exists(runner_file):
      with open(runner_file, "w") as outfile:
         def writeln(line):
            outfile.write(f"{line.rstrip()}\n")
    
         writeln(f"##                                                                    ")
         writeln(f"# @file    testsuite.py                                               ")
         writeln(f"# @version 1.0.0                                                      ")
         writeln(f"# @author  Forrest Jablonski                                          ")
         writeln(f"#                                                                     ")
         writeln(f"                                                                      ")
         writeln(f"import os                                                             ")
         writeln(f"import sys                                                            ")
         writeln(f"                                                                      ")
         writeln(f"try:                                                                  ")
         writeln(f"   # @note Grabs the first directory in the chain named unittests/.   ")
         writeln(f"   sys.path.append(os.path.join(" \
                 f"os.getcwd().split(\"unittests\")[0], \"unittests/\"))                 ")
         writeln(f"   import testsuitebase                                               ")
         writeln(f"except:                                                               ")
         writeln(f"   print(\"Cannot import testsuitebase - path set correctly?\")       ")
         writeln(f"   sys.exit(1)                                                        ")
         writeln(f"                                                                      ")
         writeln(f"try:                                                                  ")
         writeln(f"   import cppyy                                                       ")
         writeln(f"except:                                                               ")
         writeln(f"   print(\"Cannot import cppyy - did you forget to start the venv?\") ")
         writeln(f"   sys.exit(1)                                                        ")
         writeln(f"                                                                      ")
         writeln(f"\"\"\" TestSuite                                                      ")
         writeln(f"                                                                      ")
         writeln(f"@brief Collection of all tests for Random.                            ")
         writeln(f"\"\"\"                                                                ")
         writeln(f"class TestSuite(testsuitebase.TestSuiteBase):                         ")
         writeln(f"   \"\"\" __init___                                                   ")
         writeln(f"                                                                      ")
         writeln(f"   @brief Constructor.                                                ")
         writeln(f"   \"\"\"                                                             ")
         writeln(f"   def __init__(self):                                                ")
         writeln(f"      super().__init__({args.filepath}, " \
                                        f"{args.filename}, " \
                                        f"{args.SUT_name}, [])                           ")
         writeln(f"                                                                      ")
         writeln(f"   \"\"\" run                                                         ")
         writeln(f"                                                                      ")
         writeln(f"   @brief Runs the test suite.                                        ")
         writeln(f"   \"\"\"                                                             ")
         writeln(f"   def run(self):                                                     ")
         writeln(f"      from cppyy.gbl import std                                       ")
         writeln(f"      from cppyy.gbl import ym                                        ")
         writeln(f"                                                                      ")
         writeln(f"      ts = ym.ut.TestSuite()                                          ")
         writeln(f"      results = ts.run()                                              ")
         writeln(f"                                                                      ")
         writeln(f"   \"\"\" name_of_test_case_here                                      ")
         writeln(f"                                                                      ")
         writeln(f"   @brief TODO.                                                       ")
         writeln(f"   \"\"\"                                                             ")
         writeln(f"   def name_of_test_case_here(self):                                  ")
         writeln(f"      from cppyy.gbl import std                                       ")
         writeln(f"      from cppyy.gbl import ym                                        ")
         writeln(f"                                                                      ")
         writeln(f"      pass                                                            ")
         writeln(f"                                                                      ")
         writeln(f"# kick-off                                                            ")
         writeln(f"if __name__ == \"__main__\":                                          ")
         writeln(f"   if os.path.basename(os.getcwd()) != \"{args.filename}\":           ")
         writeln(f"      print(\"Needs to be run in the {args.filename}/ directory\")    ")
         writeln(f"      sys.exit(1)                                                     ")
         writeln(f"                                                                      ")
         writeln(f"   ts = TestSuite()                                                   ")
         writeln(f"   ts.run()                                                           ")

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

   os.makedirs(args.filepath, exist_ok=True)

   gen_cmake_file (args)
   gen_hdr_file   (args)
   gen_src_file   (args)
   gen_runner_file(args)

# kick-off
if __name__ == "__main__":
   main()
else:
   print("Meant to run stand-alone - not to be imported.")
   sys.exit(1)
