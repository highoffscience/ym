#!/usr/bin/python3

##
# @file    unittest_runner.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import cppyy
import unittest

""" main

@brief TODO
"""
def main():
   cppyy.add_include_path("/home/forrest/code/ym/unittests/")
   cppyy.add_include_path("/home/forrest/code/ym/unittests/common/random/")
   cppyy.include("random_ut.h")

   cppyy.add_library_path("/home/forrest/code/ym/unittests/build/")
   cppyy.load_library("librandom.so")

   r = cppyy.gbl.ym.ut.Random_UT()
   print(r.runTest_BinFrequency())

# kick-off
if __name__ == "__main__":
   main()
else:
   # TODO report error
   pass
