# Build

To build project use those commands:
``
mkdir build
cd build
cmake ..
make
``

To run application use command:
``
python3 parser.py <input JSON file path> <output JSON file path>
``

To run test after compiling run command while in /build folder:
``
ctest
``