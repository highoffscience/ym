##
# @file    runner.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import argparse
import sys

""" main

@brief TODO
"""
def main():
   parser = argparse.ArgumentParser()
   parser.add_argument("--random", help="runs the random unit test suite", action="store_true")
   self.args = parser.parse_args()

# kick-off
if __name__ == "__main__":
   main()
else:
   # TODO report error
   pass
