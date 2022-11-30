##
# @file    random_ut.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import cppyy
import os
import sys
import unittest

""" fullPath

@brief TODO
"""
def fullPath(tailPath):
   return os.path.join(os.getcwd(), tailPath)

""" main

@brief TODO
"""
def main():
   cppyy.add_include_path(fullPath("../common/"))
   cppyy.add_include_path(fullPath("/common/"))
   cppyy.include(fullPath("common/random_ut.h"))

   cppyy.add_library_path(fullPath("build/")
   cppyy.load_library(fullPath("build/common"librandom.so")

   from cppyy.gbl import std
   from cppyy.gbl import ym

   r = ym.ut.Random_UT()
   ds = r.todo_run_test()
   try:
      
      print(std.any_cast[ym.ut.ut_uint64](ds["NTotalBits"]))
   except std.bad_any_cast as bad:
      print(f"uh-oh! {bad}")

# kick-off
if __name__ == "__main__":
   main()
else:
   # TODO report error
   pass