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
   Collection of all tests for suite Pointer_Handling.
   """

   @classmethod
   def setUpClass(cls):
      """
      Acting constructor.
      """
      super().setUpBaseClass(
         suite_filepath="core/pointers",
         src_filepaths=["ym/common"])

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
      from cppyy.gbl import std # type: ignore
      from cppyy.gbl import ym  # type: ignore

      # uncomment to run test
      # results = self.run_test_case(self._testMethodName.removeprefix("test_"), assert_results=False)
      pass

   def test_SmokeTest(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type: ignore
      from cppyy.gbl import ym  # type: ignore

      results = self.run_test_case(self._testMethodName.removeprefix("test_"))
      for key in ("BoundDeref", "LooseDeref", "BoundGetRaw", "LooseGetRaw"):
         self.assertTrue(results.get[bool](key), f"{key} shall be true")

   def test_Soundness_BoundConstruction(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type: ignore
      from cppyy.gbl import ym  # type: ignore

      results = self.run_test_case(self._testMethodName.removeprefix("test_"))
      for key in ("FromRawOk", "TaggedOk", "NullCmpIsFalse", "BoolTrue", "NullRejected"):
         self.assertTrue(results.get[bool](key), f"{key} shall be true")

   def test_Soundness_BoundComparisons(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type: ignore
      from cppyy.gbl import ym  # type: ignore

      results = self.run_test_case(self._testMethodName.removeprefix("test_"))
      for key in ("SameAddrEqual", "DiffAddrNeq", "CmpConsistent", "Irreflexive", "TotalOrder"):
         self.assertTrue(results.get[bool](key), f"{key} shall be true")

   def test_Soundness_LooseNullness(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type: ignore
      from cppyy.gbl import ym  # type: ignore

      results = self.run_test_case(self._testMethodName.removeprefix("test_"))
      for key in ("DefaultNull", "ResetSet", "ResetNull", "UnwrapOk", "UnwrapThrows", "UnwrapOrValue", "UnwrapOrFallback"):
         self.assertTrue(results.get[bool](key), f"{key} shall be true")

   def test_Soundness_LooseCheckedAccess(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type: ignore
      from cppyy.gbl import ym  # type: ignore

      results = self.run_test_case(self._testMethodName.removeprefix("test_"))
      for key in ("DerefOk", "WriteOk", "InvokeValue", "InvokeVoidRan", "InvokeSide", "InvokeNullNullopt", "InvokeNullVoid", "DerefThrows", "ArrowThrows"):
         self.assertTrue(results.get[bool](key), f"{key} shall be true")

   def test_Soundness_ComparisonIsPointerBased(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type: ignore
      from cppyy.gbl import ym  # type: ignore

      results = self.run_test_case(self._testMethodName.removeprefix("test_"))
      for key in ("DiffObjectsNeq", "SameAddressEq", "NullsEq", "NonnullNeqNull", "BoundSameEq", "BoundDiffNeq"):
         self.assertTrue(results.get[bool](key), f"{key} shall be true")

   def test_Soundness_Casting(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type: ignore
      from cppyy.gbl import ym  # type: ignore

      results = self.run_test_case(self._testMethodName.removeprefix("test_"))
      for key in ("UpcastAddrSame", "ByteviewAddr", "ConstByteviewAddr", "PasskeyAddr", "LooseUpcastSame"):
         self.assertTrue(results.get[bool](key), f"{key} shall be true")

# kick-off
if __name__ == "__main__":
   TestSuite.runSuite()
else:
   TestSuite.runSuite()
