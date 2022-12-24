##
# @file    testsuitebase.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import os
import sys

try:
   import cppyy
except:
   print("Cannot import cppyy - started the venv?")
   sys.exit(1)

class TestSuiteBase:
   """
   @brief Base class representing a test suite.
   """

   def __init__(self, filepath: str,
                      filename: str,
                      SUT_name: str,
                      dependent_filepaths: list):
      """
      @brief Constructor.

      TODO params
      """

      self.filepath = filepath
      self.filename = filename
      self.SUT_name = SUT_name
      self.dependent_filepaths = dependent_filepaths

      self.ut_rootpath = os.path.dirname(os.path.abspath(__file__))
      self.rootpath    = os.path.join(self.ut_rootpath, "../../")

      self.SUTpath    = os.path.join(self.rootpath,    self.filepath)
      self.ut_SUTpath = os.path.join(self.ut_rootpath, self.filepath, self.filename)

      # self.dependent_SUT_tail_paths = dependent_SUT_tail_paths
      # self.dependent_SUT_paths      = list(map(lambda tail_path: os.path.join(self.ut_root_path, tail_path), self.dependent_SUT_tail_paths))

      # self.basename = os.path.basename(self.ut_SUT_path)

      self.configCppyy()

   def configCppyy(self):
      """
      @brief Configures the test suite environment.
      """

      cppyy.add_include_path(os.path.join(self.rootpath, "ym/common/"))
      cppyy.add_include_path(os.path.join(self.ut_rootpath, "common/"))
      cppyy.add_include_path(self.SUTpath    )
      cppyy.add_include_path(self.ut_SUTpath )

      cppyy.include(os.path.join(self.ut_SUTpath, "testsuite.h"))

      cppyy.add_library_path(os.path.join(self.ut_rootpath, "build/", self.filepath))
      cppyy.load_library(    os.path.join(self.ut_rootpath, "build/", self.filepath, f"lib{self.filename}"))

      # for tail_path in self.self.dependent_SUT_tail_paths:
      #    cppyy.add_library_path(os.path.join(self.ut_root_path, "build/", tail_path))
      #    cppyy.add_library_path(os.path.join(self.ut_root_path, "build/", tail_path, f"lib{os.path.basename(tail_path)}"))

   def run(self, test_case_names:list=None):
      """
      @brief Method to be overriden that runs the test suite.
      """

      raise NotImplementedError()

# kick-off
if __name__ == "__main__":
   print("Meant to be imported - not to be run stand-alone")
   sys.exit(1)
