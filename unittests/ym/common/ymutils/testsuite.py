##
# @file    testsuite.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import sys

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
   Collection of all tests for YmUtils.
   """

   @classmethod
   def setUpClass(cls):
      """
      Acting constructor.
      """
      super().setUpBaseClass(
         filepath="ym/common/",
         filename="ymutils")

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

      # uncomment to run test
      # results = self.run_test_case("InteractiveInspection")

   def test_PtrToIntConversion(self):
      """
      Analyzes results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("PtrToIntConversion")
      self.assertEqual(results.get[int]("Val"), 7, "Ptr to int yielded unexpected value")

   def test_BoundedPtrClass(self):
      """
      Analyzes results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("BoundedPtrClass")

      self.assertEqual(results.get[int]("Ptr_1"), 9, "Bounded ptr does not have expected value")
      self.assertEqual(results.get[str]("Name"), "Torchic", "Bounded ptr does not have expected value")

   def test_BinarySearch(self):
      """
      Analyzes results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("BinarySearch")
      self.assertTrue(results.get[bool]("ElementFound"), "Search failed to find correct element")

   def test_PtrCast(self):
      """
      Analyzes results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("PtrCast")
      self.assertTrue(results.get[bool]("True"), "Failed to compile")

# kick-off
if __name__ == "__main__":
   TestSuite.runSuite()
else:
   TestSuite.runSuite()
