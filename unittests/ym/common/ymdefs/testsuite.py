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
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      # uncomment to run test
      # results = self.run_test_case(self._testMethodName.removeprefix("test_"), assert_results=False)
      pass

   def test_SmokeTest(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case(self._testMethodName.removeprefix("test_"), assert_results=False) # pylint: disable=unused-variable

   def test_BigFiveDeleteMacros(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case(self._testMethodName.removeprefix("test_"))
      self.assertTrue(results.get[bool]("Defined"), "Macros not defined")

   def test_OverloadMacros(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case(self._testMethodName.removeprefix("test_"))
      self.assertTrue(results.get[bool]("DefinedAndWorks"), "Macro overloads not behaving as expected")

   def test_Func_getNBits(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case(self._testMethodName.removeprefix("test_"))
      self.assertTrue(results.get[bool]("CorrectNBits"), "ym_getNBits() does not behave as expected")

   def test_Func_empty(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case(self._testMethodName.removeprefix("test_"))
      self.assertTrue(results.get[bool]("CorrectResults"), "ym_empty() does not behave as expected")

   def test_PrimitiveDefs(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case(self._testMethodName.removeprefix("test_"))
      self.assertTrue(results.get[bool]("Defined"), "De facto primitives not defined")

# kick-off
if __name__ == "__main__":
   TestSuite.runSuite()
else:
   TestSuite.runSuite()
