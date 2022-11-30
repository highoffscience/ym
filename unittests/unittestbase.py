##
# @file    unittestbase.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import argparse
import os
import sys

""" UnitTestBase

@brief TODO
"""
class UnitTestBase:
   def __init__(self):
      parser = argparse.ArgumentParser()
      parser.add_argument("--random", help="runs the random unit test suite", action="store_true")
      self.args = parser.parse_args()

# kick-off
if __name__ == "__main__":
   print("Meant to be imported - not to be run stand-alone")
   sys.exit(1)
