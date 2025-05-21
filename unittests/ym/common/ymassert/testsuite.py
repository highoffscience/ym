##
# @file    testsuite.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import argparse
import os
import sys
import unittest

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
   @brief Collection of all tests for SUT ymassert_Base.
   """

   @classmethod
   def setUpClass(cls):
      """
      @brief Acting constructor.
      """

      super().setUpBaseClass(filepath="ym/common",
                             filename="ymasssert")

   @classmethod
   def tearDownClass(cls):
      """
      @brief Acting destructor.
      """
      super().tearDownBaseClass()

   def setUp(self):
      """
      @brief Set up logic that is run before each test.
      """
      pass

   def tearDown(self):
      """
      @brief Tear down logic that is run after each test.
      """
      pass

   def test_InteractiveInspection(self):
      """
      @brief Analyzes results from test case.

      @param results -- Results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("InteractiveInspection")

   def test_What(self):
      """
      @brief Analyzes results from test case.

      @param results -- Results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("What")
   
      self.assertTrue(results.get[bool]("GAM"), f"Msg not formatted as unexpected")

# def load_tests(loader, standard_tests, pattern):
#    """
#    This hook is used by unittest to load test cases manually.
#    Automatically called by unittest framework.

#    Args:
#       loader:         TODO
#       standard_tests: TODO
#       pattern:        TODO

#    Returns:
#       TestSuite: The instantiated unittest suite.
#    """

#    suite = unittest.TestSuite()
#    return suite

# kick-off
if __name__ == "__main__":
   if os.path.basename(os.getcwd()) != "ymassert":
      print("Needs to be run in the ymassert/ directory")
      sys.exit(1)

   parser = argparse.ArgumentParser()
   parser.add_argument("--unittestdir", required=True, help="Absolute path of unittest directory")
   parser.add_argument("--projrootdir", required=True, help="Absolute path of project directory")
   parser.add_argument("--binarydir",   required=True, help="Absolute path to build directory")
   args = parser.parse_args()

   testsuitebase.TestSuiteBase.unittestdir = args.unittestdir
   testsuitebase.TestSuiteBase.projrootdir = args.projrootdir
   testsuitebase.TestSuiteBase.builddir    = args.builddir

   unittest.main()
