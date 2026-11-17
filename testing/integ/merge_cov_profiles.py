##
# @file    merge_cov_profiles.py
# @version 1.0.0
# @author  Forrest Jablonski
#

"""
Script to merge coverage profiles - to be called from cmake.
"""

import ympyutils

import argparse
import json
import pathlib

def main():
   """
   Merges and displays coverage files already obtained.
   """
   parser = argparse.ArgumentParser()
   parser.add_argument("--builddir", required=True, help="Absolute path to build directory", type=pathlib.Path)
   parser.add_argument("--libname",  required=True, help="Library to run test against")
   args = parser.parse_args()

   profile_files_str = ""
   with open(f"{args.builddir / 'profiles' / 'cache.json'}", mode="r") as f:
      cachedata = json.load(f)
      for prof_file in cachedata[args.libname]:
         profile_files_str += f" {prof_file}"

   MERGED_PROFILE_FILE: pathlib.Path = args.builddir / "profiles" / f"{args.libname}.profdata"

   ympyutils.runCmd(f"llvm-profdata merge {profile_files_str} -o {MERGED_PROFILE_FILE}")
   ympyutils.runCmd(f"llvm-cov show {args.builddir / 'customlibs' / args.libname} " \
      f"-instr-profile={MERGED_PROFILE_FILE} " \
       "-use-color "                           \
       "-format=html "                         \
      f"-output-dir={args.builddir / 'profiles' / f'{args.libname}-covprofiles'} ")

# kick-off
if __name__ == "__main__":
   main()
