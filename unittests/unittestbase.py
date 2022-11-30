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
   def __init__(self, SUT_tail_path: str, dependent_SUT_tail_paths: list):

      self.ym_root_path = "/home/forrest/code/ym/"
      self.ut_root_path = os.path.join(self.ym_root_path, "unittests/")

      self.SUT_tail_path = SUT_tail_path
      self.ut_SUT_path   = os.path.join(self.ut_root_path, self.SUT_tail_path)

      self.dependent_SUT_tail_paths = dependent_SUT_tail_paths
      self.dependent_SUT_paths      = list(map(lambda s: os.path.join(TODO)))

      self.basename         = os.path.basename(self.ut_SUT_path)

      self.configCppyy()

   def configCppyy(self):
      cppyy.add_include_path(self.ym_root_path)
      cppyy.add_include_path(self.ut_root_path)
      cppyy.add_include_path(self.ut_SUT_path)

      cppyy.include(os.path.join(self.ut_SUT_path, f"{self.basename}_ut.h"))

      cppyy.add_library_path(os.path.join(self.ut_root_path, "build/", self.ut_SUT_tail_path))
      cppyy.load_library(fullPath("build/common"librandom.so")

# kick-off
if __name__ == "__main__":
   print("Meant to be imported - not to be run stand-alone")
   sys.exit(1)
