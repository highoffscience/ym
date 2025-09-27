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
   Collection of all tests for ArgParser.
   """

   @classmethod
   def setUpClass(cls):
      """
      Acting constructor.
      """
      super().setUpBaseClass(
         filepath="ym/common/",
         filename="argparser")

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
      pass

   def test_BasicParse(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case("BasicParse")

      expectedArgs = ["Input", "Output", "Clean", "Build", "Key", "Denial"]

      exc = results.get[bool]("Exc")
      self.assertFalse(exc, "Unexpected exception happened")

      for arg in expectedArgs:
         val = results.get[bool](arg)
         self.assertTrue(val, f"{arg} arg failed to parse")

   def test_FlagIntegrity(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case("FlagIntegrity")

      expectedArgs = ["Verbose", "Width"]

      exc = results.get[bool]("Exc")
      self.assertFalse(exc, "Unexpected exception happened")

      for arg in expectedArgs:
         val = results.get[bool](arg)
         self.assertTrue(val, f"{arg} arg failed to parse")

   def test_SizeOfArg(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case("SizeOfArg")

      size = results.get[std.size_t]("Size")
      self.assertEqual(size, 32, "Unexpected Arg size")

# kick-off
if __name__ == "__main__":
   TestSuite.runSuite()
else:
   TestSuite.runSuite()
