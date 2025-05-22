##
# @file    run_unittest.py
# @version 1.0.0
# @author  Forrest Jablonski
#

"""
Script that controls running unittests - to be called from cmake.
"""

import ympyutils as ympy

import argparse
import json
import os

def cacheCovProfiles(
      args: argparse.ArgumentParser,
      LLVM_PROFILE_FILE: str):
   """
   Caches all coverage profile names with their associated instrumented library.

   Args:
      args:              Command line arguments.
      LLVM_PROFILE_FILE: Name of preprocessed coverage profile file.
   """

   cachedata = None
   cachefile = f"{args.binarydir}/profiles/cache.json"

   if os.path.isfile(cachefile):
      with open(cachefile, mode="r") as f:
         cachedata = json.load(f)

   if not cachedata:
      cachedata = {}
   if args.libraryname not in cachedata:
      cachedata[args.libraryname] = []
   if LLVM_PROFILE_FILE not in cachedata[args.libraryname]:
      cachedata[args.libraryname].append(LLVM_PROFILE_FILE)

   with ympy.open_into_dir(cachefile, mode="w") as f:
      json.dump(cachedata, f, indent=3)

def main():
   """
   Runs the given unittest and conditionally runs coverage.
   """
   parser = argparse.ArgumentParser()
   parser.add_argument("--unittestdir", required=True, help="Absolute path to unittest directory")
   parser.add_argument("--projrootdir", required=True, help="Absolute path of project directory")
   parser.add_argument("--builddir",    required=True, help="Absolute path to build directory")
   parser.add_argument("--suitename",   required=True, help="Qualified testsuite name")
   parser.add_argument("--libraryname", required=True, help="Library to run test against")
   parser.add_argument("--covenabled",  required=True, help="TRUE/FALSE enables/disables coverage mode")
   args = parser.parse_args()

   args.covenabled = True if (args.covenabled == "TRUE") else False

   LLVM_PROFILE_FILE = ""
   if args.covenabled:
      LLVM_PROFILE_FILE = f"{args.binarydir}/profiles/{args.suitename}.profraw"
      cacheCovProfiles(args, LLVM_PROFILE_FILE)

   os.environ["LLVM_PROFILE_FILE"] = LLVM_PROFILE_FILE

   ympy.runCmd(f"python -m unittest {args.suitename}.testsuite " \
      f"--unittestdir={args.unittestdir} --projrootdir={args.projrootdir} --builddir={args.builddir}",
      cwd=args.unittestdir)

   if args.covenabled:
      MERGED_PROFILE_FILE = LLVM_PROFILE_FILE.replace(".profraw", ".profdata")
      ympy.runCmd(f"llvm-profdata merge {LLVM_PROFILE_FILE} -o {MERGED_PROFILE_FILE}")
      ympy.runCmd(f"llvm-cov show {args.binarydir}/customlibs/{args.libraryname} " \
         f"-instr-profile={MERGED_PROFILE_FILE} " \
          "-use-color "                           \
          "-format=html "                         \
         f"-output-dir={args.binarydir}/profiles/{args.suitename}-covprofiles ")

if __name__ == "__main__":
   main()
