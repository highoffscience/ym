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
   Collection of all tests for Ops.
   """

   @classmethod
   def setUpClass(cls):
      """
      Acting constructor.
      """

      super().setUpBaseClass(filepath="ym/common/",
                             filename="ops")

   @classmethod
   def tearDownClass(cls):
      """
      Acting destructor.
      """
      pass

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

   def test_Casting(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore
      
      results = self.run_test_case("Casting")

      self.assertTrue(results.get[bool]("Val_char"), "char cast failed")

      self.assertTrue(results.get[bool]("Val_int8" ), "int8  cast failed")
      self.assertTrue(results.get[bool]("Val_int16"), "int16 cast failed")
      self.assertTrue(results.get[bool]("Val_int32"), "int32 cast failed")
      self.assertTrue(results.get[bool]("Val_int64"), "int64 cast failed")

      self.assertTrue(results.get[bool]("Val_uint8" ), "uint8  cast failed")
      self.assertTrue(results.get[bool]("Val_uint16"), "uint16 cast failed")
      self.assertTrue(results.get[bool]("Val_uint32"), "uint32 cast failed")
      self.assertTrue(results.get[bool]("Val_uint64"), "uint64 cast failed")

      self.assertTrue(results.get[bool]("Val_float32"), "float32 cast failed")
      self.assertTrue(results.get[bool]("Val_float64"), "float64 cast failed")
      self.assertTrue(results.get[bool]("Val_float80"), "float80 cast failed")

   def test_BadCasting(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case("BadCasting")

      for result in results.get[std.vector[bool]]("BadCasts_char"):
         self.assertTrue(result, "char cast failed to fail")

      for result in results.get[std.vector[bool]]("BadCasts_int8"):
         self.assertTrue(result, "int8 cast failed to fail")
      for result in results.get[std.vector[bool]]("BadCasts_int16"):
         self.assertTrue(result, "int16 cast failed to fail")
      for result in results.get[std.vector[bool]]("BadCasts_int32"):
         self.assertTrue(result, "int32 cast failed to fail")
      for result in results.get[std.vector[bool]]("BadCasts_int64"):
         self.assertTrue(result, "int64 cast failed to fail")

      for result in results.get[std.vector[bool]]("BadCasts_uint8"):
         self.assertTrue(result, "uint8 cast failed to fail")
      for result in results.get[std.vector[bool]]("BadCasts_uint16"):
         self.assertTrue(result, "uint16 cast failed to fail")
      for result in results.get[std.vector[bool]]("BadCasts_uint32"):
         self.assertTrue(result, "uint32 cast failed to fail")
      for result in results.get[std.vector[bool]]("BadCasts_uint64"):
         self.assertTrue(result, "uint64 cast failed to fail")

      for result in results.get[std.vector[bool]]("BadCasts_flt32"):
         self.assertTrue(result, "float32 cast failed to fail")
      for result in results.get[std.vector[bool]]("BadCasts_flt64"):
         self.assertTrue(result, "float64 cast failed to fail")
      for result in results.get[std.vector[bool]]("BadCasts_flt80"):
         self.assertTrue(result, "float80 cast failed to fail")

# kick-off
if __name__ == "__main__":
   if os.path.basename(os.getcwd()) != "ops":
      print("Needs to be run in the ops/ directory")
      sys.exit(1)

   unittest.main()
