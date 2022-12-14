##
# @file    runner.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import argparse
import sys

import ym.common.random.testsuite

""" main

@brief TODO
"""
def main():
   parser = argparse.ArgumentParser()
   parser.add_argument("--all",    help="runs all unit test suites",       action="store_true")
   parser.add_argument("--random", help="runs the random unit test suite", action="store_true")
   args = parser.parse_args()

   if (args.all or args.random): ym.common.random.testsuite.TestSuite().run()

# kick-off
if __name__ == "__main__":
   main()
else:
   print("Meant to run stand-alone - not to be imported")
   sys.exit(1)
