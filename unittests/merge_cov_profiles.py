##
# @file    run_unittest.py
# @version 1.0.0
# @author  Forrest Jablonski
#

"""
   Script to merge coverage profiles - to be called from cmake.
"""

import ympyutils

import argparse
import json

def main():
   parser = argparse.ArgumentParser()
   parser.add_argument("--unittestdir", required=True, help="Absolute path to unittest directory", type=str)
   parser.add_argument("--binarydir",   required=True, help="Absolute path to build directory",    type=str)
   parser.add_argument("--libraryname", required=True, help="Library to run test against",         type=str)
   args = parser.parse_args()

   profile_files = []
   with open(f"{args.binarydir}/profiles/cache.json", mode="r") as f:
      cachedata = json.load(f)
      for prof_file in cachedata[args.libraryname]:
         profile_files.append(prof_file)

   # TODO

   if args.covenabled:
      MERGED_PROFILE_FILE = LLVM_PROFILE_FILE.replace(".profraw", ".profdata")
      ympyutils.runCmd(f"llvm-profdata merge {LLVM_PROFILE_FILE} -o {MERGED_PROFILE_FILE}")
      ympyutils.runCmd(f"llvm-cov show {args.binarydir}/customlibs/{args.libraryname} " \
         f"-instr-profile={MERGED_PROFILE_FILE} " \
          "-use-color "                           \
          "-format=html "                         \
         f"-output-dir={args.binarydir}/profiles/{args.libraryname}-covprofiles ")

if __name__ == "__main__":
   main()
