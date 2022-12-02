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
   basename = os.path.basename(args.path)
   cmake_file = os.path.join(args.path, "CMakeLists.txt")
   if not os.path.exists(cmake_file):
      with open(cmake_file, "w") as outfile:
         outfile.write(f"##\n")
         outfile.write(f"# AUTO-GENERATED\n")
         outfile.write(f"#\n")
         outfile.write(f"# @file    CMakeLists.txt\n")
         outfile.write(f"# @version 1.0.0\n")
         outfile.write(f"# @author  Forrest Jablonski\n")
         outfile.write(f"#\n")
         outfile.write(f"\n")
         outfile.write(f"cmake_minimum_required(VERSION 3.16)\n")
         outfile.write(f"\n")
         outfile.write(f"project(UnitTest VERSION 1.0.0 DESCRIPTION \"Builds unittest shared libraries\")\n")
         outfile.write(f"\n")
         outfile.write(f"set(CMAKE_CXX_STANDARD 20)\n")
         outfile.write(f"set(CMAKE_CXX_STANDARD_REQUIRED True)\n")
         outfile.write(f"\n")
         outfile.write(f"set(RootDir           ${{CMAKE_CURRENT_SOURCE_DIR}}/../../../)\n")
         outfile.write(f"set(CommonDir         ${{RootDir}}/common/)\n")
         outfile.write(f"set(UnitTestDir       ${{RootDir}}/unittests/)\n")
         outfile.write(f"set(CommonUnitTestDir ${{UnitTestDir}}/common/)\n")
         outfile.write(f"\n")
         outfile.write(f"add_library({basename} SHARED)\n")
         outfile.write(f"\n")
         outfile.write(f"target_sources({basename} PRIVATE\n")
         outfile.write(f"   ${{CommonDir}}/{basename}.cpp\n")
         outfile.write(f"   ${{UnitTestDir}}/unittestbase.cpp\n")
         outfile.write(f"   ${{CommonUnitTestDir}}/{basename}/{basename}_ut.cpp)\n")
         outfile.write(f"\n")
         outfile.write(f"set_target_properties({basename} PROPERTIES VERSION ${{PROJECT_VERSION}})\n")
         outfile.write(f"set_target_properties({basename} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${{CMAKE_BINARY_DIR}}/ym/{args.path})\n")
         outfile.write(f"\n")
         outfile.write(f"target_compile_options({basename} PRIVATE -Werror -Wall -Wextra -Ofast)\n")
         outfile.write(f"\n")
         outfile.write(f"target_include_directories({basename} PRIVATE ${{CommonDir}})\n")
         outfile.write(f"target_include_directories({basename} PRIVATE ${{UnitTestDir}})\n")
         outfile.write(f"target_include_directories({basename} PRIVATE ${{CommonUnitTestDir}})\n")

""" gen_hdr_file

@brief Generates header skeleton file.
"""
def gen_hdr_file(args):
   basename = os.path.basename(args.path)
   hdr_file =os.path.join(args.path, f"{basename}_ut.h")
   if not os.path.exists(hdr_file):
      with open(hdr_file, "w") as outfile:
         outfile.write(f"/**\n")
         outfile.write(f" * @file    {basename}_ut.h\n")
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
         outfile.write(f"/** {args.name}_UT\n")
         outfile.write(f"*\n")
         outfile.write(f"* @brief Test suite for structure {args.name}.\n")
         outfile.write(f"*/\n")
         outfile.write(f"class {args.name}_UT : public UnitTestBase\n")
         outfile.write(f"{{\n")
         outfile.write(f"public:\n")
         outfile.write(f"   explicit {args.name}_UT(void);\n")
         outfile.write(f"   virtual ~{args.name}_UT(void) = default;\n")
         outfile.write(f"\n")
         outfile.write(f"   // YM_UT_TESTCASE(name_of_test_case_here);\n")
         outfile.write(f"}};\n")
         outfile.write(f"\n")
         outfile.write(f"}} // ym::ut\n") 

""" gen_src_file

@brief Generates source skeleton file.
"""
def gen_src_file(args):
   basename = os.path.basename(args.path)
   src_file = os.path.join(args.path, f"{basename}_ut.cpp")
   if not os.path.exists(src_file):
      with open(src_file, "w") as outfile:
         outfile.write(f"/**\n")
         outfile.write(f" * @file    {basename}_ut.cpp\n")
         outfile.write(f" * @version 1.0.0\n")
         outfile.write(f" * @author  Forrest Jablonski\n")
         outfile.write(f" */\n")
         outfile.write(f"\n")
         outfile.write(f"#include \"ym.h\"\n")
         outfile.write(f"#include \"{basename}_ut.h\"\n")
         outfile.write(f"\n")
         outfile.write(f"#include \"{basename}.h\"\n")
         outfile.write(f"\n")
         outfile.write(f"/** {args.name}_UT\n")
         outfile.write(f"*\n")
         outfile.write(f"* @brief Constructor.\n")
         outfile.write(f"*/\n")
         outfile.write(f"ym::ut::{args.name}_UT::{args.name}_UT(void)\n")
         outfile.write(f"   : UnitTestBase(\"{args.name}\")\n")
         outfile.write(f"{{\n")
         outfile.write(f"}}\n")

""" main

@brief Sets up the unittest scaffolding.
"""
def main():
   parser = argparse.ArgumentParser()
   parser.add_argument("-p", "--path", required=True, help="path to generate unittest scaffolding in", type=str)
   parser.add_argument("-n", "--name", required=True, help="structure to generate unittest scaffolding for", type=str)
   args = parser.parse_args()

   if not os.path.exists(args.path):
      os.mkdir(args.path)
      gen_cmake_file(args)
      gen_hdr_file(args)
      gen_src_file(args)

# kick-off
if __name__ == "__main__":
   main()
else:
   print("Meant to run stand-alone - not to be imported.")
   sys.exit(1)
