##
# @file    testsuitebase.py
# @version 1.0.0
# @author  Forrest Jablonski
#

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
   @brief Base class representing a test suite.
   """

   @classmethod
   def setUpBaseClass(cls, filepath: str,
                           filename: str,
                           SUT_name: str,
                           dependent_filepaths: list):
      """
      @brief Acting constructor.

      @param filepath            -- Path of SUT relative to unittests/ directory.
      @param filename            -- Name of SUT file.
      @param SUT_name            -- Name of SUT.
      @param dependent_filepaths -- Relative paths of dependent SUT's.
      """

      cls.filepath = filepath
      cls.filename = filename
      cls.SUT_name = SUT_name
      cls.dependent_filepaths = dependent_filepaths

      cls.ut_rootpath = os.path.dirname(os.path.abspath(__file__))
      cls.rootpath    = os.path.join(cls.ut_rootpath, "../../")

      cls.SUTpath    = os.path.join(cls.rootpath,    cls.filepath)
      cls.ut_SUTpath = os.path.join(cls.ut_rootpath, cls.filepath, cls.filename)

      # self.dependent_SUT_tail_paths = dependent_SUT_tail_paths
      # self.dependent_SUT_paths      = list(map(lambda tail_path: os.path.join(self.ut_root_path, tail_path), self.dependent_SUT_tail_paths))

      # self.basename = os.path.basename(self.ut_SUT_path)

      cls.configCppyy()

   @classmethod
   def configCppyy(cls):
      """
      @brief Configures the test suite environment.
      """

      cppyy.add_include_path(os.path.join(cls.rootpath, "ym/common/"))
      cppyy.add_include_path(os.path.join(cls.ut_rootpath, "common/"))
      cppyy.add_include_path(cls.SUTpath    )
      cppyy.add_include_path(cls.ut_SUTpath )

      cppyy.include(os.path.join(cls.ut_SUTpath, "testsuite.h"))

      cppyy.add_library_path(os.path.join(cls.ut_rootpath, "build/", cls.filepath))
      cppyy.load_library(    os.path.join(cls.ut_rootpath, "build/", cls.filepath, f"lib{cls.filename}"))

      # for tail_path in self.self.dependent_SUT_tail_paths:
      #    cppyy.add_library_path(os.path.join(self.ut_root_path, "build/", tail_path))
      #    cppyy.add_library_path(os.path.join(self.ut_root_path, "build/", tail_path, f"lib{os.path.basename(tail_path)}"))

   def run_test_case(self, test_case_name):
      """
      @brief Runs the specified test case.

      @param test_case_name -- Name of test case to run.

      @returns DataShuttle -- Dictionary containing results of test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      class TempExc:
         def what(self):
            return None

      ts = ym.ut.TestSuite()
      results = None
      saved_exc = TempExc()
      try:
         results = ts.runTestCase(test_case_name)
      except Exception as exc:
         saved_exc = exc
      except: # catch non-std based exceptions (not guaranteed to have what())
         self.assertTrue(False, f"Unhandled exception in test case {test_case_name}")

      self.assertTrue(type(saved_exc) is TempExc, f"Unhandled exception in test case {test_case_name} - {saved_exc.what()}")
      self.assertTrue(results, "Results is None")

      return results

# kick-off
if __name__ == "__main__":
   print("Meant to be imported - not to be run stand-alone")
   sys.exit(1)
