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
         outfile.write(f"##\n")
         outfile.write(f"# @file    CMakeLists.txt\n")
         outfile.write(f"# @version 1.0.0\n")
         outfile.write(f"# @author  Forrest Jablonski\n")
         outfile.write(f"#\n")
         outfile.write(f"\n")
         outfile.write(f"cmake_minimum_required(VERSION 3.16)\n")
         outfile.write(f"\n")
         outfile.write(f"project({args.filename} VERSION 1.0.0 DESCRIPTION \"Builds {args.filename} shared libraries\")\n")
         outfile.write(f"\n")
         outfile.write(f"set(CMAKE_CXX_STANDARD 20)\n")
         outfile.write(f"set(CMAKE_CXX_STANDARD_REQUIRED True)\n")
         outfile.write(f"\n")
         outfile.write(f"set(RootDir        ${{CMAKE_CURRENT_SOURCE_DIR}}/../../)\n")
         outfile.write(f"set(CommonDir      ${{RootDir}}/ym/common/)\n")
         outfile.write(f"set(UnitTestDir    ${{RootDir}}/ym/unittests/)\n")
         outfile.write(f"set(SUTDir         ${{RootDir}}/{args.filepath}/)\n")
         outfile.write(f"set(SUTUnitTestDir ${{UnitTestDir}}/{args.filepath}/{args.filename}/)\n")
         outfile.write(f"\n")
         outfile.write(f"add_library({args.filename} SHARED)\n")
         outfile.write(f"\n")
         outfile.write(f"target_sources({args.filename} PRIVATE\n")
         outfile.write(f"   ${{SUTDir}}/{args.filename}.cpp\n")
         outfile.write(f"   ${{UnitTestDir}}/testcase.cpp\n")
         outfile.write(f"   ${{UnitTestDir}}/datashuttle.cpp\n")
         outfile.write(f"   ${{UnitTestDir}}/unittestbase.cpp\n")
         outfile.write(f"   ${{SUTUnitTestDir}}/{args.filename}_ut.cpp)\n")
         outfile.write(f"\n")
         outfile.write(f"set_target_properties({args.filename} PROPERTIES VERSION ${{PROJECT_VERSION}})\n")
         outfile.write(f"set_target_properties({args.filename} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${{CMAKE_BINARY_DIR}}/{args.filepath})\n")
         outfile.write(f"\n")
         outfile.write(f"target_compile_options({args.filename} PRIVATE -Werror -Wall -Wextra -O2)\n")
         outfile.write(f"\n")
         outfile.write(f"target_include_directories({args.filename} PRIVATE ${{CommonDir}})\n")
         outfile.write(f"target_include_directories({args.filename} PRIVATE ${{UnitTestDir}})\n")
         outfile.write(f"target_include_directories({args.filename} PRIVATE ${{SUTDir}})\n")
         outfile.write(f"target_include_directories({args.filename} PRIVATE ${{SUTUnitTestDir}})\n")

""" gen_hdr_file

@brief Generates header skeleton file.
"""
def gen_hdr_file(args):
   hdr_file = os.path.join(args.filepath, args.filename, f"{args.filename}_ut.h")
   if not os.path.exists(hdr_file):
      with open(hdr_file, "w") as outfile:
         outfile.write(f"/**\n")
         outfile.write(f" * @file    {args.filename}_ut.h\n")
         outfile.write(f" * @version 1.0.0\n")
         outfile.write(f" * @author  Forrest Jablonski\n")
         outfile.write(f" */\n")
         outfile.write(f"\n")
         outfile.write(f"#pragma once\n")
         outfile.write(f"\n")
         outfile.write(f"#include \"ym_ut.h\"\n")
         outfile.write(f"\n")
         outfile.write(f"#include \"unittestbase.h\"\n")
         outfile.write(f"\n")
         outfile.write(f"namespace ym::ut\n")
         outfile.write(f"{{\n")
         outfile.write(f"\n")
         outfile.write(f"/** {args.SUT_name}_UT\n")
         outfile.write(f"*\n")
         outfile.write(f"* @brief Test suite for structure {args.SUT_name}.\n")
         outfile.write(f"*/\n")
         outfile.write(f"class {args.SUT_name}_UT : public UnitTestBase\n")
         outfile.write(f"{{\n")
         outfile.write(f"public:\n")
         outfile.write(f"   explicit {args.SUT_name}_UT(void);\n")
         outfile.write(f"   virtual ~{args.SUT_name}_UT(void) = default;\n")
         outfile.write(f"\n")
         outfile.write(f"   // YM_UT_TESTCASE(name_of_test_case_here);\n")
         outfile.write(f"}};\n")
         outfile.write(f"\n")
         outfile.write(f"}} // ym::ut\n") 

""" gen_src_file

@brief Generates source skeleton file.
"""
def gen_src_file(args):
   src_file = os.path.join(args.filepath, args.filename, f"{args.filename}_ut.cpp")
   if not os.path.exists(src_file):
      with open(src_file, "w") as outfile:
         outfile.write(f"/**\n")
         outfile.write(f" * @file    {args.filename}_ut.cpp\n")
         outfile.write(f" * @version 1.0.0\n")
         outfile.write(f" * @author  Forrest Jablonski\n")
         outfile.write(f" */\n")
         outfile.write(f"\n")
         outfile.write(f"#include \"ym.h\"\n")
         outfile.write(f"#include \"{args.filename}_ut.h\"\n")
         outfile.write(f"\n")
         outfile.write(f"#include \"{args.filename}.h\"\n")
         outfile.write(f"\n")
         outfile.write(f"/** {args.SUT_name}_UT\n")
         outfile.write(f"*\n")
         outfile.write(f"* @brief Constructor.\n")
         outfile.write(f"*/\n")
         outfile.write(f"ym::ut::{args.SUT_name}_UT::{args.SUT_name}_UT(void)\n")
         outfile.write(f"   : UnitTestBase(\"{args.SUT_name}\")\n")
         outfile.write(f"{{\n")
         outfile.write(f"   addTestCase<name_of_test_case_here>();\n")
         outfile.write(f"}}\n")
         outfile.write(f"\n")
         outfile.write(f"\n")
         outfile.write(f"/** run\n")
         outfile.write(f" *\n")
         outfile.write(f" * @brief TODO.\n")
         outfile.write(f" * \n")
         outfile.write(f" * @note TODO.\n")
         outfile.write(f" *\n")
         outfile.write(f" * @return DataShuttle -- Important values acquired during run of test case.\n")
         outfile.write(f" */\n")
         outfile.write(f"auto ym::ut::{args.SUT_name}_UT::name_of_test_case_here::run(DataShuttle const & InData) -> DataShuttle\n")
         outfile.write(f"{{\n")
         outfile.write(f"   return {{}};\n")
         outfile.write(f"}}\n")

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
   parser.add_argument("--filepath", required=True, help="path file exists in",          type=str)
   parser.add_argument("--filename", required=True, help="name of file",                 type=str)
   parser.add_argument("--SUT_name", required=True, help="name of structure under test", type=str)
   args = parser.parse_args()

   os.makedirs(args.filepath, exist_ok=True)

   gen_cmake_file(args)
   gen_hdr_file(args)
   gen_src_file(args)

# kick-off
if __name__ == "__main__":
   main()
else:
   print("Meant to run stand-alone - not to be imported.")
   sys.exit(1)
