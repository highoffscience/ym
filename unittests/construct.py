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

def getLinkMap():
   """
   @brief TODO

   @note
      ym.common
         ym.common.argparser -> ym.common
         ym.common.rng       -> ym.common
         ym.common.timer     -> ym.common
   """

   return { # all
      "@action": run_unittests, "@links": {
         "ym": {
            "@action": run_unittests, "@links": {
               "common": {
                  "@action": run_unittests, "@links": {
                     "argparser": {"@action": run_unittest, "@links": "ym.common"},
                     "rng"      : {"@action": run_unittest, "@links": "ym.common"},
                     "timer"    : {"@action": run_unittest, "@links": "ym.common"}
                  }
               },
               "hsm": {
                  "@action": run_unittests, "@links": {
                     "hsm": {"@action": run_unittest, "@links": "ym.hsm"}
                  }
               }
            }
         }
      }
   }

def run_unittest(args,
                 target:  str,
                 cov_obj: str):
   """
   @brief Runs specific unittest.

   @param args    -- Command line arguments.
   @param target  -- Specified subset of unittests to run.
   @param cov_obj -- Object to run coverage against (if applicable).
   """

   # testsuitebase makes decision on this variable so always set it to desired state
   if args.cov:
      os.environ["LLVM_PROFILE_FILE"] = f"./covbuild/profiles/{target}.profraw"
   else:
      if "LLVM_PROFILE_FILE" in os.environ:
         del os.environ["LLVM_PROFILE_FILE"]

   runCmd(f"python -m unittest {target}.testsuite", per_line_action_func=print)

def run_unittests(args,
                  target:   str,
                  link_map: dict):
   """
   @brief Runs all unittests specified in link_map.

   @param args     -- Command line arguments.
   @param target   -- Specified subset of unittests to run.
   @param link_map -- Map containing the subset of unittests to run.
   """

   for key, link in link_map.items():
      link["@action"](args, f"{target}.{key}", link["@links"])

def main():
   """
   @brief TODO

   @note Available options:
      --target
         all
         ym.common.argparser_unittest -> ym.common
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

         cov_flag = "-DYM_COV_ENABLED=True" if args.cov else ""
         runCmd(f"cmake .. {cov_flag}", cwd=build_dir, per_line_action_func=print)

      runCmd(f"cmake --build . --target {args.target}", cwd=build_dir, per_line_action_func=print)

   if args.run:
      link_map = getLinkMap()
      if args.target != "all": # find desired subset of unittests
         for link in args.target.split("."):
            link_map = link_map["@links"][link]
      link_map["@action"](args.target, link_map["@links"])

   # ----------------------------------------

   # testsuitebase makes decision on this variable so always set it to desired state
   if args.cov:
      os.environ["LLVM_PROFILE_FILE"] = f"./covbuild/profiles/{args.target}.profraw"
   else:
      if "LLVM_PROFILE_FILE" in os.environ:
         del os.environ["LLVM_PROFILE_FILE"]

   if args.run:
      if args.target == "all":
         # this could be accomplished with unittest discovery, but less verbose this way
         for root, dirs, files in os.walk("ym/"):
            if "testsuite.py" in files:
               runCmd(f"python -m unittest {root.replace('/', '.')}.testsuite", per_line_action_func=print)
      else:
         runCmd(f"python -m unittest {args.target}", per_line_action_func=print)
         
      if args.cov:
         root_filename = os.path.splitext(os.environ["LLVM_PROFILE_FILE"])[0]
         runCmd(f"llvm-profdata merge {root_filename}.profraw -o {root_filename}.profdata")

         obj_file = f"./covbuild/customlibs/{args.object}"
         runCmd(f"llvm-cov show {obj_file} -instr-profile={root_filename}.profdata -use-color -format=html -output-dir={root_filename}-covprofiles")
   
# kick-off
if __name__ == "__main__":
   #main()
   getLinkMap()
else:
   print("Meant to run stand-alone - not to be imported.")
   sys.exit(1)
