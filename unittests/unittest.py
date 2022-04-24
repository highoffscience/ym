#!/usr/bin/python3

##
# @author Forrest Jablonski
#

import argparse
import datetime
import os
import subprocess
import sys

##
#
#
class TestManager:
   ##
   #
   #
   def __init__(self):
      timeStamp = datetime.datetime.now().strftime("%Y_%m_%d_%H:%M:%S")
      resultsDir = os.path.join("results/", timeStamp)
      self.logger = open(f"unittest_log_{timeStamp}.txt", "w")

   ##
   #
   #
   def __del__(self):
      self.logger.close()

   ##
   #
   #
   def defaultTestHandler(self):
      self.runCmd(["cmake", "--build", "build/"])

   ##
   #
   #
   def randomTestHandler(self):
      pass

   ##
   #
   #
   def initTestInfo(self):
      return {
         "random": [0]
      }

   ##
   #
   #
   def log(self, msg):
      self.logger.write(f"{msg}\n")

   ##
   #
   #
   def runCmd(self, cmd, timeout_sec=60):
      proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
      try:
         out, err = proc.communicate(timeout_sec)
      except:
         proc.kill()
         self.log(f"Command '{cmd}' timed out!")
      return out.decode().strip().split("\n")

   ##
   #
   #
   def runTests(self):
      pass

##
#
#
def main():
   mgr = TestManager()
   baseDir = "./common/random_test_tools/dieharder-3.31.1/"
   mgr.log(mgr.runCmd(["make", "-C", f"{baseDir}"]))
   #mgr.log(runCmd("./common/rand_test_tools/dieharder-3.31.1/"))

##
#
#
if __name__ == "__main__":
   main()
else:
   print("Module meant to run stand-alone - not to be imported.")
   sys.exit(1)
