##
# @file    runner.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import argparse
import sys

import ym.common.random.testsuite

def main():
   """
   @brief Parses supplied arguments and runs specified test suites.
   """
   parser = argparse.ArgumentParser()
   parser.add_argument("--all",    help="runs all unit test suites",       action="store_true")

   parser.add_argument("--random", help="runs the random unit test suite", action="store_true")

   parser.add_argument("--test_cases", help="runs the specified tests in the given test suite",
                       nargs="+", type=list, default=None)
   args = parser.parse_args()

   if (args.all or args.random): ym.common.random.testsuite.TestSuite().run(args.test_cases)

# kick-off
if __name__ == "__main__":
   main()
else:
   print("Meant to run stand-alone - not to be imported")
   sys.exit(1)
