#!/usr/bin/python

##
# @file    unittest_runner.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import cppyy
import unittest

""" main

@brief TODO
"""
def main():
   cppyy.add_include_path("/home/forrest/code/ym/unittests/")
   cppyy.add_include_path("/home/forrest/code/ym/unittests/common/random/")
   cppyy.include("random_ut.h")

   cppyy.add_library_path("/home/forrest/code/ym/unittests/build/")
   cppyy.load_library("librandom.so")

   r = cppyy.gbl.ym.ut.Random_UT()
   ds = r.todo_run_test()
   try:
      print(cppyy.gbl.std.any_cast[cppyy.gbl.ym.ut.ut_uint64](ds["NTotalBits"]))
   except cppyy.gbl.std.bad_any_cast as bad:
      print(f"uh-oh! {bad}")

# kick-off
if __name__ == "__main__":
   main()
else:
   # TODO report error
   pass

# #!/usr/bin/python3

##
# # @file    random_unittest.py
# # @version 1.0.0
# # @author  Forrest Jablonski
# #

# import argparse
# import datetime
# import os
# import subprocess
# import sys

# ##
# #
# #
# class TestManager:
#    ##
#    #
#    #
#    def __init__(self):
#       timeStamp = datetime.datetime.now().strftime("%Y_%m_%d_%H:%M:%S")
#       resultsDir = os.path.join("results/", timeStamp)
#       #self.logger = open(f"unittest_log_{timeStamp}.txt", "w")
#       self.logger = open(f"unittest_log.txt", "w")

#    ##
#    #
#    #
#    def __del__(self):
#       self.logger.close()

#    ##
#    #
#    #
#    def defaultTestHandler(self):
#       self.runCmd(["cmake", "--build", "build/"])

#    ##
#    #
#    #
#    def randomTestHandler(self):
#       pass

#    ##
#    #
#    #
#    def initTestInfo(self):
#       return {
#          "random": [0]
#       }

#    ##
#    #
#    #
#    def log(self, msg):
#       self.logger.write(f"{msg}\n")

#    ##
#    #
#    #
#    def runCmd(self, cmd, timeout_sec=60):
#       proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
#       try:
#          out, err = proc.communicate(timeout_sec)
#       except:
#          proc.kill()
#          self.log(f"Command '{cmd}' timed out!")
#       return out.decode().strip().split("\n")

#    ##
#    #
#    #
#    def runTests(self):
#       pass

# ##
# #
# #
# def main():
#    mgr = TestManager()
#    baseDir = "./common/test_tools/dieharder-3.31.1/"
#    mgr.log(mgr.runCmd(["make", "-C", f"{baseDir}"]))
#    #mgr.log(runCmd("./common/rand_test_tools/dieharder-3.31.1/"))

# ##
# #
# #
# def test_main():
#    mgr = TestManager()
#    print(mgr.runCmd(["cmake", "--build", "build/", "--target", "clean"]))
#    print(mgr.runCmd(["cmake", "--build", "build/"]))
#    print(mgr.runCmd(["./build/unittest"]))

# ##
# #
# #
# if __name__ == "__main__":
#    #main()
#    test_main()
# else:
#    print("Module meant to run stand-alone - not to be imported.")
#    sys.exit(1)
