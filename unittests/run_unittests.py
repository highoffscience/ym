##
# @file    run_unittests.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import ympyutils

import argparse

def main():
   parser = argparse.ArgumentParser()
   parser.add_argument("--unittestdir", required=True, help="Absolute path to unittest directory",  type=str)
   parser.add_argument("--targetname",  required=True, help="Testsuite names",                      type=str)
   parser.add_argument("--libraryname", required=True, help="Library to run test against",          type=str)
   # parser.add_argument("--cov",         required=True, help="TRUE/FALSE enables/disables cov mode", type=str)
   args = parser.parse_args()

   ympyutils.runCmd(f"python -m unittest ${args.targetname}.testsuite")

if __name__ == "__main__":
   main()
