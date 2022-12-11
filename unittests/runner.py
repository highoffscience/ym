##
# @file    runner.py
# @version 1.0.0
# @author  Forrest Jablonski
#

TODO # instead of ym:ut it should be ut::ym

import argparse
import sys

import ym.common.random.random_ut

""" main

@brief TODO
"""
def main():
   parser = argparse.ArgumentParser()
   parser.add_argument("--random", help="runs the random unit test suite", action="store_true")
   args = parser.parse_args()

   if (args.random):
      ym.common.random.random_ut.Random_UT().run()

# kick-off
if __name__ == "__main__":
   main()
else:
   print("Meant to run stand-alone - not to be imported")
   sys.exit(1)
