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
   @brief Collection of all tests for suite YmUtils.
   """

   @classmethod
   def setUpClass(cls):
      """
      @brief Acting constructor.
      """

      super().setUpBaseClass(filepath="ym/common/",
                             filename="ymutils")

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

      # uncomment to run test
      # results = self.run_test_case("InteractiveInspection")

   def test_PtrToIntConversion(self):
      """
      @brief Analyzes results from test case.

      @param results -- Results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("PtrToIntConversion")

      self.assertEqual(results.get[int]("Val"), 7, "Ptr to int yielded unexpected value")

   def test_BoundedPtrClass(self):
      """
      @brief Analyzes results from test case.

      @param results -- Results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("BoundedPtrClass")

      self.assertEqual(results.get[int]("Ptr_1"), 9, "Bounded ptr does not have expected value")
      self.assertEqual(results.get[str]("Name"), "Torchic", "Bounded ptr does not have expected value")

   def test_BinarySearch(self):
      """
      @brief Analyzes results from test case.

      @param results -- Results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("BinarySearch")

      self.assertTrue(results.get[bool]("ElementFound"), "Search failed to find correct element")

# kick-off
if __name__ == "__main__":
   if os.path.basename(os.getcwd()) != "ymutils":
      print("Needs to be run in the ymutils/ directory")
      sys.exit(1)

   unittest.main()
