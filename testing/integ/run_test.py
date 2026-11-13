##
# @file    run_test.py
# @version 1.0.0
# @author  Forrest Jablonski
#

"""
Script that controls running integration tests.
"""

import ympyutils as ympy

import argparse
import json
import os
import pathlib

def cacheCovProfiles(
      args: argparse.ArgumentParser,
      LLVM_PROFILE_FILE: str | os.PathLike[str]):
   """
   Caches all coverage profile names with their associated instrumented library.

   Args:
      args:              Command line arguments.
      LLVM_PROFILE_FILE: Name of preprocessed coverage profile file.
   """

   cachedata = {}
   cachefile: pathlib.Path = args.builddir / "profiles" / "cache.json"

   if cachefile.exists():
      with open(cachefile, mode="r") as f:
         cachedata = json.load(f)

   if args.libname not in cachedata:
      cachedata[args.libname] = []
   if str(LLVM_PROFILE_FILE) not in cachedata[args.libname]:
      cachedata[args.libname].append(str(LLVM_PROFILE_FILE))

   with ympy.open_into_dir(cachefile, mode="w") as f:
      json.dump(cachedata, f, indent=3)

def main():
   """
   Runs the given test and conditionally runs coverage.
   """
   parser = argparse.ArgumentParser()
   parser.add_argument("--testdir",   required=True,       help="Absolute path to integration test directory", type=pathlib.Path)
   parser.add_argument("--projdir",   required=True,       help="Absolute path of project directory",          type=pathlib.Path)
   parser.add_argument("--builddir",  required=True,       help="Absolute path to build directory",            type=pathlib.Path)
   parser.add_argument("--suitename", required=True,       help="Qualified testsuite name")
   parser.add_argument("--libname",   required=True,       help="Library to run test against")
   parser.add_argument("--cov",       action="store_true", help="Enables/disables coverage mode")
   args = parser.parse_args()

   LLVM_PROFILE_FILE = pathlib.Path()
   if args.cov:
      LLVM_PROFILE_FILE = args.builddir / "profiles" / f"{args.suitename}.profraw"
      cacheCovProfiles(args, LLVM_PROFILE_FILE)
      os.environ["LLVM_PROFILE_FILE"] = str(LLVM_PROFILE_FILE)

   ympy.runCmd(f"python -m {args.suitename}.testsuite -v " \
      f"--testdir={args.testdir} " \
      f"--projdir={args.projdir} " \
      f"--builddir={args.builddir}",
      cwd=args.testdir, per_line_action_func=print)

   if args.cov:
      MERGED_PROFILE_FILE: pathlib.Path = str(LLVM_PROFILE_FILE).replace(".profraw", ".profdata")
      ympy.runCmd(f"llvm-profdata merge {LLVM_PROFILE_FILE} -o {MERGED_PROFILE_FILE}")
      ympy.runCmd(f"llvm-cov show {args.builddir / 'customlibs' / args.libname} " \
         f"-instr-profile={MERGED_PROFILE_FILE} " \
          "-use-color "                           \
          "-format=html "                         \
         f"-output-dir={args.builddir / 'profiles' / args.suitename / '-covprofiles'} ")

# kick-off
if __name__ == "__main__":
   main()
