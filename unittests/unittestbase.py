##
# @file    unittestbase.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import cppyy
import os
import sys

""" UnitTestBase

@brief TODO
"""
class UnitTestBase:
   """ __init__
   
   @brief TODO
   """
   def __init__(self, official_name: str, SUT_tail_path: str, dependent_SUT_tail_paths: list):

      self.official_name = official_name

      self.ym_root_path = "/home/forrest/code/ym/"
      self.ut_root_path = os.path.join(self.ym_root_path, "unittests/")

      self.SUT_tail_path = SUT_tail_path
      self.ut_SUT_path   = os.path.join(self.ut_root_path, self.SUT_tail_path)

      self.dependent_SUT_tail_paths = dependent_SUT_tail_paths
      self.dependent_SUT_paths      = list(map(lambda tail_path: os.path.join(self.ut_root_path, tail_path), self.dependent_SUT_tail_paths))

      self.basename = os.path.basename(self.ut_SUT_path)

      self.configCppyy()

   def configCppyy(self):
      cppyy.add_include_path(self.ym_root_path)
      cppyy.add_include_path(self.ut_root_path)
      cppyy.add_include_path(self.ut_SUT_path)

      cppyy.include(os.path.join(self.ut_SUT_path, f"{self.basename}_ut.h"))

      cppyy.add_library_path(os.path.join(self.ut_root_path, "build/", self.SUT_tail_path))
      cppyy.load_library(    os.path.join(self.ut_root_path, "build/", self.SUT_tail_path, f"lib{self.basename}"))

      for tail_path in self.self.dependent_SUT_tail_paths:
         cppyy.add_library_path(os.path.join(self.ut_root_path, "build/", tail_path))
         cppyy.add_library_path(os.path.join(self.ut_root_path, "build/", tail_path, f"lib{os.path.basename(tail_path)}"))

   def run(self):
      raise NotImplementedError()

# kick-off
if __name__ == "__main__":
   print("Meant to be imported - not to be run stand-alone")
   sys.exit(1)