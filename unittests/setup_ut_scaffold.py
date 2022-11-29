##
# @file    setup_ut_scaffold.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import argparse
import os
import sys

""" gen_cmake_file

@brief TODO
"""
def gen_cmake_file(directory):
   if not os.path.exists(directory):
      basename = os.path.basename(directory)
      with open(f"{directory}/CMakeLists.txt", "w") as outfile:
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
         outfile.write(f"\n")
         outfile.write(f"target_compile_options({basename} PRIVATE -Werror -Wall -Wextra -Ofast)\n")
         outfile.write(f"\n")
         outfile.write(f"target_include_directories({basename} PRIVATE ${{CommonDir}})\n")
         outfile.write(f"target_include_directories({basename} PRIVATE ${{UnitTestDir}})\n")
         outfile.write(f"target_include_directories({basename} PRIVATE ${{CommonUnitTestDir}})\n")

""" gen_header_file

@brief TODO
"""
def gen_header_file(directory):
   if not os.path.exists(directory):
      basename = os.path.basename(directory)
      with open(f"{directory}/{basename}_ut.h", "w") as outfile:

""" main

@brief TODO
"""
def main():
   parser = argparse.ArgumentParser()
   parser.add_argument("-d", "--directory", help="path to generate unittest scaffolding in", type=str)
   parser.add_argument("-n", "--name", help="structure to generate unittest scaffolding for", type=str)
   args = parser.parse_args()

   print(args.file)

# kick-off
if __name__ == "__main__":
   main()
else:
   print("Meant to run stand-alone - not to be imported")
   sys.exit(1)
