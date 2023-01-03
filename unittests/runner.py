##
# @file    runner.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import argparse
import sys

import ym.common.random.testsuite
import ym.common.textlogger.testsuite
import ym.common.timer.testsuite

def main():
   """
   @brief Parses supplied arguments and runs specified test suites.
   """
   parser = argparse.ArgumentParser()
   parser.add_argument("--all",        help="runs all unit test suites",            action="store_true")

   parser.add_argument("--random",     help="runs the random unit test suite",      action="store_true")
   parser.add_argument("--textlogger", help="runs the text logger unit test suite", action="store_true")
   parser.add_argument("--timer",      help="runs the timer unit test suite",       action="store_true")

   parser.add_argument("--test_cases", help="runs the specified tests in the given test suite",
                       nargs="+", type=str, default=None)
   args = parser.parse_args()

   if (args.all or args.random    ): ym.common.random    .testsuite.TestSuite().run(args.test_cases)
   if (args.all or args.textlogger): ym.common.textlogger.testsuite.TestSuite().run(args.test_cases)
   if (args.all or args.timer     ): ym.common.timer     .testsuite.TestSuite().run(args.test_cases)

# kick-off
if __name__ == "__main__":
   main()
else:
   print("Meant to run stand-alone - not to be imported")
   sys.exit(1)
