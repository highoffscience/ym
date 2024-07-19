#!/usr/bin/env python3

##
# @file    construct.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import argparse
import os
import pathlib
import shutil
import sys

sys.path.append("..")
from ympyutils import *

def main():
   """
   @brief TODO
   """

   parser = argparse.ArgumentParser()
   parser.add_argument("--target",      "-t", default="all",       help="Target to build")
   parser.add_argument("--object",      "-o", default="all",       help="Object file to run coverage against")
   parser.add_argument("--cov",         "-c", action="store_true", help="Activates coverage mode")
   parser.add_argument("--reconfigure", "-f", action="store_true", help="Cleans build directory then configures")
   parser.add_argument("--run",         "-r", action="store_true", help="Runs associated unittest")

   args = parser.parse_args()

   build_dir_str = "./covbuild/" if args.cov else "./build"
   with pathlib.Path(build_dir_str) as build_dir:
      if args.reconfigure:
         if build_dir.exists() and build_dir.is_dir():
            shutil.rmtree(build_dir)
         build_dir.mkdir()

      if args.reconfigure:
         cov_flag = "-DYM_COV_ENABLED=True" if args.cov else ""
         runCmd(f"cmake .. {cov_flag}", cwd=build_dir, per_line_action_func=print)
      runCmd(f"cmake --build . --target {args.target}", cwd=build_dir, per_line_action_func=print)

   # testsuitebase makes decision on this variable so always set it to desired state
   if args.cov:
      os.environ["LLVM_PROFILE_FILE"] = f"./covbuild/profiles/{args.target}.profraw"
   else:
      if "LLVM_PROFILE_FILE" in os.environ:
         del os.environ["LLVM_PROFILE_FILE"]

   if args.run:
      if args.target == "all":
         for root, dirs, files in os.walk("ym/"):
            if "testsuite.py" in files:
               runCmd(f"python -m unittest {args.target.replace("/", ".")}.testsuite", per_line_action_func=print)
      else:
         runCmd(f"python -m unittest {args.target}.testsuite", per_line_action_func=print)
         if args.cov:
            root_filename = os.path.splitext(os.environ["LLVM_PROFILE_FILE"])[0]
            runCmd(f"llvm-profdata merge {root_filename}.profraw -o {root_filename}.profdata")

            obj_file = f"./covbuild/customlibs/{args.object}"
            runCmd(f"llvm-cov show {obj_file} -instr-profile={root_filename}.profdata -use-color -format=html -output-dir={root_filename}-covprofiles")
   
# kick-off
if __name__ == "__main__":
   main()
else:
   print("Meant to run stand-alone - not to be imported.")
   sys.exit(1)
