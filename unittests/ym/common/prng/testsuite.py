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
   @brief Collection of all tests for SUT PRNG.
   """

   @classmethod
   def setUpClass(cls):
      """
      @brief Acting constructor.
      """

      super().setUpBaseClass(filepath="ym/common",
                             filename="prng",
                             SUT_name="PRNG")

   @classmethod
   def tearDownClass(cls):
      """
      @brief Acting destructor.
      """
      pass

   def setUp(self):
      """
      @brief Set up logic that is run before each test.
      """
      pass

   def tearDown(self):
      """
      @brief Tear down logic that is run after each test.
      """
      pass

   def test_ZerosAndOnes(self):
      """
      @brief Analyzes results from test case.

      @param results -- Results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("ZerosAndOnes")

      set_bit_vector = results.get[std.vector[std.pair[ym.utdefs.uint64, ym.utdefs.uint64]]]("SetBitVector")
      for set_bits in set_bit_vector:
         nTrials    = set_bits.first
         kSuccesses = set_bits.second
         assumedProb = 0.5
         probOfResultOrWorse = bn.cdf(kSuccesses, nTrials, assumedProb)
         print(f"For {nTrials} trials, prob of {kSuccesses} or worse {(probOfResultOrWorse*100):0.2f}%")

   def test_UniformBins(self):
      """
      @brief Analyzes results from test case.

      @param results -- Results from test case.
      """

      from cppyy.gbl import std
      from cppyy.gbl import ym

      results = self.run_test_case("UniformBins")

      for bin_name in ["u32", "u64", "f32", "f64"]:
         bin_counts = np.array(results.get[std.vector[ym.utdefs.uint64]](f"{bin_name}Bins"))
         print(f"std of {bin_name} bins {bin_counts.std()}")
         print(f"min of {bin_name} bins {bin_counts.min()}")
         print(f"max of {bin_name} bins {bin_counts.max()}")
         print(f"ave of {bin_name} bins {bin_counts.mean()}")

# kick-off
if __name__ == "__main__":
   if os.path.basename(os.getcwd()) != "prng":
      print("Needs to be run in the prng/ directory")
      sys.exit(1)

   unittest.main()
