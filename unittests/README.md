Install instructions for unittest directory
(cppyy doesn't work with python3.11, continue with 3.10)

can experiment with "$jupyter lab <name-of-file>" after activating venv

$ python -m venv venv/
$ . ./venv/bin/activate
$ python -m pip install -r requirements.txt
$ mkdir build/ | covbuild/
$ cd build/ | covbuild/
$ cmake .. [-DCOV_ENABLED=True|False]
$ cmake --build . [--target <suite-name>[-run]]

eg...
$ cmake --build . --target ym.common.ymdefs[-run]

To clean, eg...
$ cmake --build . --target clean

Produced libraries are placed in customlibs/
The "_run" option will build the testsuite(s) and then run it
If running for coverage it will optionally produce coverage files in profiles/

To add a new unittest suite:
Use setup_ut_scaffold.py
Edit generated files as appropriate
Edit CMakeLists.txt

If the suite requires extra care to test you can add a custom build.cmake to the test suite directory

// -----------------------------------------------------------------------------

Unittests require to be run in the venv
$ . ./venv/bin/activate

To run a unittest manually, eg...
$ python -m unittest ym.common.ymdefs.testsuite

To run a specific test, eg...
$ python -m unittest ym.common.ymdefs.testsuite.TestSuite.test_InteractiveInspection

Test suites can also be run in the directory, eg...
$ python testsuite.py

// -----------------------------------------------------------------------------

<https://clang.llvm.org/docs/SourceBasedCodeCoverage.html>
<https://llvm.org/docs/CommandGuide/llvm-cov.html>

To prepare testing for coverage do the following:
$ cd covbuild/
$ cmake .. -DCOV_ENABLED=True
$ cmake --build . --target clean

The scripts use llvm-cov show, but could also use llvm-cov report...
$ llvm-cov report <desired-obj-file> -instr-profile=<testsuite>.profdata

// -----------------------------------------------------------------------------

                  a/                         a/
                 .  .                       .  .
               .      .                   .      .
              b/        c/               b/        c/
             .         .  .             .         .  .
          b0..bn     .      .        b0..bn     .      .
                  c0..cn     d/              c0..cn    d/
                             .                         .
                             e/                        e/
                             .                         .
                          e0..en                    e0..en

Example source directory structure is above. a/ - e/ are directories, a0..an are files.
There are 3 types of directories build-wise:
   1) File-Level directories
   2) Library    directories
   3) Container  directories

Type 1:
   Each source file will have it's own directory for unittesting. These directories
      are called File-Level directories. They may contain their own build.cmake
      script if building the unittest is non-trivial.

Type 2:
   Every source file will have a directory it lives in. At the unittest level every
      source directory that contains a source file will be a library directory. This
      means all direct source files (not nested) will be packaged into a shared object.
      This increases modularity and build times. Each unittest file in the directory
      will link to this shared object.
   These directories contain a build.cmake that build the shared object as well as
      defines the unittest targets for each of the source files, assuming that source
      file's File-Level unittest directory doesn't contain it's own build.cmake.

Type 3:
   These directories don't contain any files, just other directories. Nested shared
      objects will link to the parent shared object.
