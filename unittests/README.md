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
