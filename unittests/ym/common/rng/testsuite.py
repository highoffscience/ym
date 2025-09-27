##
# @file    testsuite.py
# @version 1.0.0
# @author  Forrest Jablonski
#

import os
import sys
import unittest

import numpy as np
from scipy.stats import binom as bn

try:
   # @note Grabs the first directory in the chain named unittests/.
   sys.path.append(os.path.join(os.getcwd().split("unittests")[0], "unittests/"))
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
   Collection of all tests for PRNG.
   """

   @classmethod
   def setUpClass(cls):
      """
      Acting constructor.
      """

      super().setUpBaseClass(filepath="ym/common",
                             filename="rng")

   @classmethod
   def tearDownClass(cls):
      """
      Acting destructor.
      """
      pass

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

   def test_ZerosAndOnes(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case("ZerosAndOnes")

      set_bit_vector = results.get[std.vector[std.pair[ym.uint64, ym.uint64]]]("SetBitVector")
      for set_bits in set_bit_vector:
         nTrials    = set_bits.first
         kSuccesses = set_bits.second
         assumedProb = 0.5
         probOfResultOrWorse = bn.cdf(kSuccesses, nTrials, assumedProb)
         print(f"For {nTrials} trials, prob of {kSuccesses} or worse {(probOfResultOrWorse*100):0.2f}%")

   def test_UniformBins(self):
      """
      Analyzes results from test case.
      """
      from cppyy.gbl import std # type:ignore
      from cppyy.gbl import ym  # type:ignore

      results = self.run_test_case("UniformBins")

      for bin_name in ["u32", "u64", "f32", "f64"]:
         bin_counts = np.array(results.get[std.vector[ym.uint64]](f"{bin_name}Bins"))
         print(f"std of {bin_name} bins {bin_counts.std()}")
         print(f"min of {bin_name} bins {bin_counts.min()}")
         print(f"max of {bin_name} bins {bin_counts.max()}")
         print(f"ave of {bin_name} bins {bin_counts.mean()}")

# kick-off
if __name__ == "__main__":
   if os.path.basename(os.getcwd()) != "rng":
      print("Needs to be run in the rng/ directory")
      sys.exit(1)

   unittest.main()
