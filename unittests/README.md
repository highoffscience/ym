Install instructions for unittest directory
(cppyy doesn't work with python3.11, continue with 3.10)

can experiment with "$jupyter lab <name-of-file>" after activating venv

$ python -m venv venv/
$ . ./venv/bin/activate
$ python -m pip install -r requirements.txt
$ mkdir build/ | covbuild/
$ cd build/ | covbuild/
$ cmake .. [-DYM_COV_ENABLED=True|False]
$ cmake --build . [--target <suite-name>[_run]]

eg...
$ cmake --build . --target ym.common.ymdefs[_run]

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
$ cmake .. -DYM_COV_ENABLED=True
$ cmake --build . --target clean

TODO
llvm-profdata merge doesn't merge all the files

The scripts use llvm-cov show, but could also use llvm-cov report...
$ llvm-cov report <desired-obj-file> -instr-profile=<testsuite>.profdata
