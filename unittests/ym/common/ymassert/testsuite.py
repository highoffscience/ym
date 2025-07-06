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
   Collection of all tests for SUT ymassert_Base.
   """

   @classmethod
   def setUpClass(cls):
      """
      Acting constructor.
      """
      super().setUpBaseClass(
         filepath="ym/common/",
         filename="ymassert")

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

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("InteractiveInspection")
      self.assertTrue(results.get[bool]("Errored"), "Exception was not thrown as expected")

   def test_What(self):
      """
      Analyzes results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      # results = self.run_test_case("What")
   
      # self.assertTrue(results.get[bool]("GAM"), f"Msg not formatted as unexpected")

def main():
   """
   Setup.
   """

   # parser = argparse.ArgumentParser()
   # parser.add_argument("--unittestdir", required=True, help="Absolute path of unittest directory")
   # parser.add_argument("--projrootdir", required=True, help="Absolute path of project directory")
   # parser.add_argument("--builddir",    required=True, help="Absolute path to build directory")
   # args = parser.parse_args()

   # testsuitebase.TestSuiteBase.customBaseInit(args.unittestdir, args.projrootdir, args.builddir)
   testsuitebase.TestSuiteBase.customBaseInit(
      "/home/forrest/code/ym/unittests",
      "/home/forrest/code", "/home/forrest/code/ym/unittests/build")

   unittest.main()

# kick-off
if __name__ == "__main__":
   main()
