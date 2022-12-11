##
# @file    random_ut.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import os
import sys

try:
   sys.path.append(os.path.join(os.getcwd().split("unittests")[0], "unittests/"))
   import unittestbase
except:
   print("Cannot import unittesbase - did you set the correct parth?")
   sys.exit(1)

try:
   import cppyy
except:
   print("Cannot import cppyy - did you forget to start the venv?")
   sys.exit(1)

""" Random_UT

@brief TODO
"""
class Random_UT(unittestbase.UnitTestBase):
   """ __init___
   
   @brief TODO
   """
   def __init__(self):
      super().__init__("ym/common/", "random", "Random", [])

   def run(self):
      from cppyy.gbl import std
      from cppyy.gbl import ym

      r = ym.ut.Random_UT()
      ds = r.runTestCase("ZerosAndOnes")
      nTotalBits = ds.get[ym.ut.utUint64]("NTotalBits", 0)
      nSetBits   = ds.get[ym.ut.utUint64]("NSetBits", 0)
      print(f"nTotalBits = {nTotalBits}")
      print(f"nSetBits = {nSetBits}")
      print(f"Ratio = {nSetBits/nTotalBits}")

# kick-off
if __name__ == "__main__":
   if os.path.basename(os.getcwd()) != "random":
      print("Needs to be run in the random/ directory")
      sys.exit(1)

   r = Random_UT()
   r.run()
