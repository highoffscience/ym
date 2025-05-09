##
# @file    run_unittest.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import ympyutils

import argparse
import json
import os

def cacheCovProfiles(
   args: argparse.ArgumentParser,
   LLVM_PROFILE_FILE: str):
   """
   TODO switch to shinx - google style docs
   https://www.sphinx-doc.org/en/master/usage/extensions/example_google.html
   """
   cachedata = {"files": []}
   cachefile = f"{args.binarydir}/profiles/cache.json"
   if os.path.isfile(cachefile):
      with open(cachefile, mode="r") as f:
         cachedata = json.load(f)
   if LLVM_PROFILE_FILE not in cachedata["files"]:
      cachedata["files"].append(LLVM_PROFILE_FILE)
   with ympyutils.open_into_dir(cachefile, mode="w") as f:
      json.dump(cachedata, f, indent=3)

def main():
   parser = argparse.ArgumentParser()
   parser.add_argument("--unittestdir", required=True, help="Absolute path to unittest directory",       type=str)
   parser.add_argument("--binarydir",   required=True, help="Absolute path to build directory",          type=str)
   parser.add_argument("--suitename",   required=True, help="Qualified testsuite name",                  type=str)
   parser.add_argument("--libraryname", required=True, help="Library to run test against",               type=str)
   parser.add_argument("--covenabled",  required=True, help="TRUE/FALSE enables/disables coverage mode", type=str)
   args = parser.parse_args()

   args.covenabled = True if (args.covenabled == "TRUE") else False

   LLVM_PROFILE_FILE = ""
   if args.covenabled:
      LLVM_PROFILE_FILE = f"{args.binarydir}/profiles/{args.suitename}.profraw"
      
      # TODO
      # {"libname": []}
      cachedata = {"files": [], "libname": ""}
      cachefile = f"{args.binarydir}/profiles/cache.json"
      if os.path.isfile(cachefile):
         with open(cachefile, mode="r") as f:
            cachedata = json.load(f)
      if LLVM_PROFILE_FILE not in cachedata["files"]:
         cachedata["files"].append(LLVM_PROFILE_FILE)
      with ympyutils.open_into_dir(cachefile, mode="w") as f:
         json.dump(cachedata, f, indent=3)

   os.environ["LLVM_PROFILE_FILE"] = LLVM_PROFILE_FILE

   ympyutils.runCmd(f"python -m unittest {args.suitename}.testsuite", cwd=args.unittestdir)

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
