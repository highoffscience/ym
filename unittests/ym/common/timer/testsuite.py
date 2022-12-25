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
except:
   print("Cannot import cppyy - started the venv?")
   sys.exit(1)

class TestSuite(testsuitebase.TestSuiteBase):
   """
   @brief Collection of all tests for SUT Timer.
   """

   def __init__(self):
      """
      @brief Constructor.
      """
      super().__init__("ym/common/", "timer", "Timer", [])

      self.test_cases = {
         "ElapsedTime": self.analyze_results_ElapsedTime
      }

   def run(self, test_case_names:list=None):
      """
      @brief Runs the test suite.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      ts = ym.ut.TestSuite()

      if test_case_names is not None:
         for name in test_case_names:
            try:
               results = ts.runTestCase(name)
               self.test_cases[name](results)
            except:
               print(f"Something failed with test case {name}")
      else:
         all_results = ts.runAllTestCases()
         for test_case_data in all_results:
            name = test_case_data.first
            test_case_results = test_case_data.second
            self.test_cases[name](test_case_results)

   def analyze_results_ElapsedTime(self, results):
      """
      @brief Analyzes results from test case.

      @param results -- Results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      pass

# kick-off
if __name__ == "__main__":
   if os.path.basename(os.getcwd()) != "timer":
      print("Needs to be run in the timer/ directory")
      sys.exit(1)

   ts = TestSuite()
   ts.run()
