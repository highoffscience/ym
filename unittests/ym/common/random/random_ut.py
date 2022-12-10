##
# @file    random_ut.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import os
import sys
import unittestbase

try:
   import cppyy
except:
   print("Did you forget to start the venv?")
   sys.exit(1)

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
      ds = r.runTestCase("ZeroesAndOnes")
      nTotalBits = ds.get[ym.ut.ut_uint64]("NTotalBits", 0)
      print(f"nTotalBits = {nTotalBits}")

# kick-off
if __name__ == "__main__":
   print("Meant to be imported - not to be run stand-alone")
   sys.exit(1)
