##
# @file    testsuite.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import os
import sys
import unittest

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
   Collection of all tests for FileIO.
   """

   @classmethod
   def setUpClass(cls):
      """
      Acting constructor.
      """

      super().setUpBaseClass(filepath="ym/common",
                             filename="fileio")

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
      pass

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

      results = self.run_test_case("InteractiveInspection")

      print(f"--> {results.get[ym.utdefs.cchar]('E0')}")

# kick-off
if __name__ == "__main__":
   if os.path.basename(os.getcwd()) != "fileio":
      print("Needs to be run in the fileio/ directory")
      sys.exit(1)

   unittest.main()
