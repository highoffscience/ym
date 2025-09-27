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
   Collection of all tests for TextLogger.
   """

   @classmethod
   def setUpClass(cls):
      """
      Acting constructor.
      """
      super().setUpBaseClass(
         filepath="ym/common/",
         filename="textlogger")

   @classmethod
   def tearDownClass(cls):
      """
      Acting destructor.
      """
      super().tearDownBaseClass()

   def setUp(self):
      """
      Set up logic that is run before each test.
      """
      prev_files = glob.glob(os.path.join(self.unittestdir, "logs/log*.txt"))
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
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore
      
      # uncomment to run test
      # results = self.run_test_case("InteractiveInspection")
      pass

   def test_OpenAndClose(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case("OpenAndClose")

      isOpen = results.get[bool]("IsOpen")
      self.assertTrue(isOpen, "text logger failed to open")

      isClosed = results.get[bool]("IsClosed")
      self.assertTrue(isClosed, "text logger failed to close")

# kick-off
if __name__ == "__main__":
   TestSuite.runSuite()
else:
   TestSuite.runSuite()
