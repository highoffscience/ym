##
# @file    random_ut.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import cppyy
import os
import sys
import unittestbase

""" Random_UT

@brief TODO
"""
class Random_UT(unittestbase.UnitTestBase):
   """ __init___
   
   @brief TODO
   """
   def __init__(self):
      super().__init__("Random", "common/random", [])

   def run(self):
      from cppyy.gbl import std
      from cppyy.gbl import ym

      r = ym.ut.Random_UT()
      ds = r.runZerosAndOnes()
      try:
         
         print(std.any_cast[ym.ut.ut_uint64](ds["NTotalBits"]))
      except std.bad_any_cast as bad:
         print(f"uh-oh! {bad}")

# kick-off
if __name__ == "__main__":
   print("Meant to be imported - not to be run stand-alone")
   sys.exit(1)
