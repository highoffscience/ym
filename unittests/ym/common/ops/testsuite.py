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
   @brief Collection of all tests for SUT Ops.
   """

   @classmethod
   def setUpClass(cls):
      """
      @brief Acting constructor.
      """

      super().setUpBaseClass(filepath="ym/common/",
                             filename="ops",
                             SUT_name="Ops",
                             dependent_filepaths=[])

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

   def test_Casting(self):
      """
      @brief Analyzes results from test case.

      @param results -- Results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("Casting")

      # todo int8, uint8

      val_char = results.get[ym.utdefs.int8]("Val_char")
      print(f"val = {val_char}")
      # self.assertEqual(val_char, 'f', "char cast failed")

      # val_int32 = results.get[ym.utdefs.int32]("Val_int32")
      # self.assertEqual(val_int32, 65, "int32 cast failed")

# kick-off
if __name__ == "__main__":
   if os.path.basename(os.getcwd()) != "ops":
      print("Needs to be run in the ops/ directory")
      sys.exit(1)

   unittest.main()
