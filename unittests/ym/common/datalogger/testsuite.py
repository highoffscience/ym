##
# @file    testsuite.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import glob
import os
import sys

import ympyutils as ympy

try:
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
   Collection of all tests for DataLogger.
   """
   @classmethod
   def setUpClass(cls):
      """
      Acting constructor.
      """
      super().setUpBaseClass(
         filepath="ym/common",
         filename="datalogger")

   @classmethod
   def tearDownClass(cls):
      """
      Acting destructor.
      """
      pass

   def setUp(self):
      """
      Set up logic that is run before each test.
      """
      prev_files = glob.glob(os.path.join(self.unittestdir, "logs/data*.csv"))
      if prev_files:
         ympy.runCmd(f"rm -rf {' '.join(prev_files)}")

   def tearDown(self):
      """
      Tear down logic that is run after each test.
      """
      pass

   def test_InteractiveInspection(self):
      """
      Analyzes results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      ym.ymLogPushEnable(ym.VG.Error)

      # uncomment to run test
      results = self.run_test_case("InteractiveInspection")

# kick-off
if __name__ == "__main__":
   TestSuite.runSuite()
else:
   TestSuite.runSuite()
