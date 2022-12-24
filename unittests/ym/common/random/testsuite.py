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
   print("Cannot import testsuitebase - path set correctly?")
   sys.exit(1)

try:
   import cppyy

   from cppyy.gbl import std
   from cppyy.gbl import ym
except:
   print("Cannot import cppyy - started the venv?")
   sys.exit(1)

class TestSuite(testsuitebase.TestSuiteBase):
   """
   @brief Collection of all tests for SUT Random.
   """

   def __init__(self):
      """
      @brief Constructor.
      """
      super().__init__("ym/common/", "random", "Random", [])

      self.test_cases = {
         "ZerosAndOnes": self.analyze_results_ZerosAndOnes
      }

   def run(self, test_case_names:list=None):
      """
      @brief Runs the test suite or set of test cases.
      """

      ts = ym.ut.TestSuite()

      if test_case_names is not None:
         for name in test_case_names:
            try:
               results = ts.runTestCase(name)
               self.test_cases[name](results)
            except:
               print(f"Something went wrong running test case {name}")
      else:
         all_results = ts.runAllTestCases()
         for name in all_results:
            self.test_cases[name](all_results[name])

   def analyze_results_ZerosAndOnes(self, results):
      """
      @brief Analyzes results from the zeros and ones test.
      """

      nTotalBits = results.get[ym.ut.utuint64]("NTotalBits", 0)
      nSetBits   = results.get[ym.ut.utuint64]("NSetBits", 0)
      print(f"nTotalBits = {nTotalBits}")
      print(f"nSetBits = {nSetBits}")
      print(f"Ratio = {nSetBits/nTotalBits}")

# kick-off
if __name__ == "__main__":
   if os.path.basename(os.getcwd()) != "random":
      print("Needs to be run in the random/ directory")
      sys.exit(1)

   ts = TestSuite()
   ts.run()
