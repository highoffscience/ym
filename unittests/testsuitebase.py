##
# @file    testsuitebase.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import argparse
import json
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

   # these to be set before before running suite
   unittestdir = None
   projrootdir = None
   builddir    = None

   @classmethod
   def customBaseInit(cls,
         unittestdir: str,
         projrootdir: str,
         builddir: str):
      """
      Initializes variables.

      Args:
         unittestdir: Absolute path of unittest directory.
         projrootdir: Absolute path of project directory.
         builddir:    Absolute path to build directory.
      """

      cls.unittestdir = unittestdir
      cls.projrootdir = projrootdir
      cls.builddir    = builddir

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

      if not cls.unittestdir or not cls.projrootdir or not cls.builddir:
         print(f"Testsuite for file {filename} not setup correctly")
         sys.exit(1)

      cls.filepath = filepath
      cls.filename = filename

      cls.abs_ut_suite_path = os.path.join(cls.unittestdir, cls.filepath, cls.filename)
      cls.abs_src_path      = os.path.join(cls.projrootdir, cls.filepath)

      cls.processCompileCommands()
      cls.configCppyy()

   @classmethod
   def tearDownBaseClass(cls):
      """
      Acting destructor.
      """
      cls.unittestdir = None
      cls.projrootdir = None
      cls.builddir    = None

   @classmethod
   def processCompileCommands(cls):
      """
      Adds defines so cppyy and source code are on the same page.

      Notes:
         compile_commands.json is generated by cmake.

         Reason for EXPORTS exception can be found at <https://cmake.org/cmake/help/v3.0/prop_tgt/DEFINE_SYMBOL.html>.
      """
      with open(os.path.join(cls.builddir, "compile_commands.json"), "r") as ccfile:
         data = json.load(ccfile)
         for d in data:
            if d["file"].endswith(os.path.join(cls.filename, "testsuite.cpp")):
               for arg in d["command"].split():
                  if arg.startswith("-D") and not arg.endswith("_EXPORTS"):
                     cppyy.cppdef(f"#define {arg[2:].replace('=', ' ')}")

   @classmethod
   def configCppyy(cls):
      """
      Configures the test suite environment.
      """

      cppyy.add_include_path(os.path.join(cls.unittestdir, "common/"))
      cppyy.add_include_path(cls.abs_ut_suite_path)
      cppyy.add_include_path(cls.abs_src_path)

      cppyy.include(os.path.join(cls.abs_ut_suite_path, "testsuite.h"))

      cppyy.add_library_path(os.path.join(cls.builddir, "customlibs/"))
      cppyy.load_library(f"lib{os.path.join(cls.filepath, cls.filename).replace('/', '.')}-unittest")

   @classmethod
   def runSuite(cls):
      parser = argparse.ArgumentParser()
      parser.add_argument("--unittestdir", required=True, help="Absolute path of unittest directory")
      parser.add_argument("--projrootdir", required=True, help="Absolute path of project directory")
      parser.add_argument("--builddir",    required=True, help="Absolute path to build directory")
      parser.add_argument("-v", "--verbosity", action="store_true", help="Enable verbosity")
      args = parser.parse_args()

      cls.customBaseInit(args.unittestdir, args.projrootdir, args.builddir)
      
      suite = unittest.TestSuite()
      loader = unittest.TestLoader()
      suite.addTests(loader.loadTestsFromTestCase(cls))

      verbolevel = 2 if args.verbosity else 0
      runner = unittest.TextTestRunner(verbosity=verbolevel)
      runner.run(suite)

   def run_test_case(self, test_case_name: str):
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
