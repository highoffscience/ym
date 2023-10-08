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
   @brief Collection of all tests for SUT ArgParser.
   """

   @classmethod
   def setUpClass(cls):
      """
      @brief Acting constructor.
      """

      super().setUpBaseClass(filepath="ym/common/",
                             filename="argparser",
                             SUT_name="ArgParser")

   @classmethod
   def tearDownClass(cls):
      """
      @brief Acting destructor.
      """
      pass

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

   def test_BasicParse(self):
      """
      @brief Analyzes results from test case.

      @param results -- Results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("BasicParse")

      expectedArgs = ["Input", "Output", "Clean", "Build", "Key", "Denial"]

      exc = results.get[bool]("Exc")
      self.assertFalse(exc, "Unexpected exception happened")

      for arg in expectedArgs:
         val = results.get[bool](arg)
         self.assertTrue(val, f"{arg} arg failed to parse")

   def test_FlagIntegrity(self):
      """
      @brief Analyzes results from test case.

      @param results -- Results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("FlagIntegrity")

      expectedArgs = ["Verbose", "Width"]

      exc = results.get[bool]("Exc")
      self.assertFalse(exc, "Unexpected exception happened")

      for arg in expectedArgs:
         val = results.get[bool](arg)
         self.assertTrue(val, f"{arg} arg failed to parse")

# kick-off
if __name__ == "__main__":
   if os.path.basename(os.getcwd()) != "argparser":
      print("Needs to be run in the argparser/ directory")
      sys.exit(1)

   unittest.main()
