##
# @file    testsuite.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import os
import sys

try:
   # @note Grabs the first directory in the chain named unittests/.
   sys.path.append(os.path.join(os.getcwd().split("unittests")[0], "unittests/"))
   import testsuitebase
except:
   print("Cannot import unittesbase - did you set the correct path?")
   sys.exit(1)

try:
   import cppyy
except:
   print("Cannot import cppyy - did you forget to start the venv?")
   sys.exit(1)

""" TestSuite

@brief Collection of all tests for Random.
"""
class TestSuite(testsuitebase.TestSuiteBase):
   """ __init___
   
   @brief Constructor.
   """
   def __init__(self):
      super().__init__("ym/common/", "random", "Random", [])

   """ run
   
   @brief Runs the test suite.
   """
   def run(self):
      from cppyy.gbl import std
      from cppyy.gbl import ym

      ts = ym.ut.TestSuite()
      results = ts.runTestCase("ZerosAndOnes")
      nTotalBits = results.get[ym.ut.utuint64]("NTotalBits", 0)
      nSetBits   = results.get[ym.ut.utuint64]("NSetBits", 0)
      print(f"nTotalBits = {nTotalBits}")
      print(f"nSetBits = {nSetBits}")
      print(f"Ratio = {nSetBits/nTotalBits}")

   """ name_of_test_case_here
   
   @brief TODO.
   """
   def name_of_test_case_here(self):
      pass

# kick-off
if __name__ == "__main__":
   if os.path.basename(os.getcwd()) != "random":
      print("Needs to be run in the random/ directory")
      sys.exit(1)

   ts = TestSuite()
   ts.run()
