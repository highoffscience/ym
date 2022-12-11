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
   def __init__(self, filepath: str,
                      filename: str,
                      SUT_name: str,
                      dependent_filepaths: list):

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
      cppyy.add_include_path(os.path.join(self.rootpath, "ym/common/"))
      cppyy.add_include_path(self.ut_rootpath)
      cppyy.add_include_path(self.SUTpath    )
      cppyy.add_include_path(self.ut_SUTpath )

      cppyy.include(os.path.join(self.ut_SUTpath, f"{self.filename}_ut.h"))

      cppyy.add_library_path(os.path.join(self.ut_rootpath, "build/", self.filepath))
      cppyy.load_library(    os.path.join(self.ut_rootpath, "build/", self.filepath, f"lib{self.filename}"))

      # for tail_path in self.self.dependent_SUT_tail_paths:
      #    cppyy.add_library_path(os.path.join(self.ut_root_path, "build/", tail_path))
      #    cppyy.add_library_path(os.path.join(self.ut_root_path, "build/", tail_path, f"lib{os.path.basename(tail_path)}"))

   def run(self):
      raise NotImplementedError()

# kick-off
if __name__ == "__main__":
   print("Meant to be imported - not to be run stand-alone")
   sys.exit(1)
