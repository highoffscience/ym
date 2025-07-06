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
   Collection of all tests for suite YmDefs.
   """

   @classmethod
   def setUpClass(cls):
      """
      Acting constructor.
      """
      super().setUpBaseClass(
         filepath="ym/common/",
         filename="ymdefs")

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

      # results = self.run_test_case("InteractiveInspection")
      pass

   def test_BigFiveDeleteMacros(self):
      """
      Analyzes results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("BigFiveDeleteMacros")

      self.assertTrue(results.get[bool]("Defined"), "Macros not defined")

   def test_OverloadMacros(self):
      """
      Analyzes results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("OverloadMacros")

      self.assertEqual(results.get[int]("Sum"), 10, "Macro overloads not behaving as expected")

# kick-off
if __name__ == "__main__":
   TestSuite.runSuite()
else:
   TestSuite.runSuite()
