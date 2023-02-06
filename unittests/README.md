Install instructions for unittest directory

python -m venv venv/
. ./venv/bin/activate
python -m pip install -r requirements.txt
mkdir build/
cd build/
cmake ..
cd ..
cmake --build build/

To add a new unittest suite:
Use setup_ut_scaffold.py
Edit generated files as appropriate
Edit runner.py
Edit CMakeLists.txt

If the SUT requires extra care to test make sure CMakeLists.txt exists in the directory

To run a unittest, eg...
python -m unittest ym.common.textlogger.testsuite

To run a specific test, eg...
python -m unittest ym.common.textlogger.testsuite.TestSuite.test_OpenAndClose

Test suites can also be run in the directory, eg...
python testsuite.py
