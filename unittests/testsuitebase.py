##
# @file    testsuitebase.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import ympyutils as ympy

import os
import sys
import unittest

try:
   import cppyy
except:
   print("Cannot import cppyy - started the venv?")
   sys.exit(1)

class TestSuiteBase(unittest.TestCase):
   """
   Base class representing a test suite.
   """

   @classmethod
   def setUpBaseClass(cls,
         filepath: str,
         filename: str):
      """
      Acting constructor.

      Args:
         filepath: Path of suite relative to unittests/ directory.
         filename: Name of suite file.
      """

      cls.filepath    = filepath
      cls.filename    = filename

      cls.unittestdir = os.path.dirname(os.path.abspath(__file__))
      cls.rootpath    = os.path.join(cls.unittestdir, "../../")

      cls.suitepath   = os.path.join(cls.rootpath,    cls.filepath)
      cls.utsuitepath = os.path.join(cls.unittestdir, cls.filepath, cls.filename)

      cls.configCppyy()

   @classmethod
   def tearDownBaseClass(cls):
      """
      Acting destructor.
      """
      pass

   @classmethod
   def configCppyy(cls):
      """
      Configures the test suite environment.
      """

      cppyy.add_include_path(os.path.join(cls.unittestdir, "common/"))
      cppyy.add_include_path(cls.suitepath  )
      cppyy.add_include_path(cls.utsuitepath)

      cppyy.include(os.path.join(cls.utsuitepath, "testsuite.h"))

      build_path = "covbuild/" if os.environ.get("LLVM_PROFILE_FILE", default=None) else "build/"
      cppyy.add_library_path(os.path.join(cls.unittestdir, build_path, "customlibs/"))
      cppyy.load_library(f"lib{os.path.join(cls.filepath, cls.filename).replace('/', '.')}-unittest")

   def run_test_case(self,
         test_case_name: str):
      """
      Runs the specified test case.

      Args:
         test_case_name: Name of test case to run.

      Returns:
         DataShuttle: Dictionary containing results of test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      class NullExc:
         def what(self):
            return None

      ts = ym.ut.TestSuite()
      results = None
      saved_exc = NullExc() # NullExc is so the format string can be parsed (parsed does not mean executed)
      try:
         results = ts.runTestCase(test_case_name)
      except Exception as exc:
         saved_exc = exc
      except: # catch non-std based exceptions (not guaranteed to have what())
         self.assertTrue(False, f"Unhandled exception in test case {test_case_name}")

      self.assertTrue(type(saved_exc) is NullExc, f"Unhandled exception in test case {test_case_name} - {saved_exc.what()}")
      
      # results will also eval to None if the dictionary is empty
      self.assertTrue(results, "Results is None")

      return results

# kick-off
if __name__ == "__main__":
   print("Meant to be imported - not to be run stand-alone")
   sys.exit(1)
