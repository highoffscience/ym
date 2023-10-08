Install instructions for unittest directory
(cppyy doesn't work with python3.11, continue with 3.10)

can experiment with "$jupyter lab <name-of-file>" after activating venv

$ python -m venv venv/
$ . ./venv/bin/activate
$ python -m pip install -r requirements.txt
$ mkdir build/
$ cd build/
$ cmake ..
$ cd ..
$ cmake --build build/ [--target <suite-name>]
$ eg...
$ cmake --build build/ [--target ym.common.textlogger]

To add a new unittest suite:
Use setup_ut_scaffold.py
Edit generated files as appropriate
Edit runner.py
Edit CMakeLists.txt

If the SUT requires extra care to test make sure CMakeLists.txt exists in the directory

To run a unittest, eg...
$ python -m unittest ym.common.textlogger.testsuite

To run a specific test, eg...
$ python -m unittest ym.common.textlogger.testsuite.TestSuite.test_OpenAndClose

Test suites can also be run in the directory, eg...
$ python testsuite.py

<https://clang.llvm.org/docs/SourceBasedCodeCoverage.html>
<https://llvm.org/docs/CommandGuide/llvm-cov.html>

To prepare testing for coverage do the following:
$ rm -rf covbuild/*
$ . ./set_cov_env.sh
Run the desired unit test, eg (TODO below instructions need work)
   $ LLVM_PROFILE_FILE="<filename>.profraw"
   $ python -m unittest ...
$ llvm-profdata merge default.profraw -o default.profdata
$ llvm-cov show <desired-obj-file> -instr-profile=stringops.profdata

The desired obj file can be the .so or a .o that comprises the .so
