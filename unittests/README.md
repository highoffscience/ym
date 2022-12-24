Install instructions for unittest directory

python -m venv venv/
. ./venv/bin/activate
python -m pip install -r requirements.txt
mkdir build/
cd build/
cmake ..
cd ..
cmake --build build/
