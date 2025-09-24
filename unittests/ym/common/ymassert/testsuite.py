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
   Collection of all tests for YmAssert.
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
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore
      
      # uncomment to run test
      # results = self.run_test_case("InteractiveInspection")
      # self.assertTrue(results.get[bool]("Errored"), "Exception was not thrown as expected")
      pass

   def test_What(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case("What")
      self.assertTrue(results.get[bool]("ExpectedMsg"), f"Msg not formatted as unexpected")

   def test_Assertion(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case("Assertion")
   
      self.assertTrue(results.get[bool]("ExpectedFalseAssert"), f"Expected assert")
      self.assertTrue(results.get[bool]("ExpectedTrueAssert" ), f"Did not expect assert")

# kick-off
if __name__ == "__main__":
   TestSuite.runSuite()
else:
   TestSuite.runSuite()
