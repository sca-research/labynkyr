**Requirements**

* Boost >= 1.58 (earlier versions may suffice, but have not been tested)
* CMake 2.8.12
* libunittest++ 1.4.0
* maven >= 3.3.9 (for Matlab/Java implementation)

Boost is used to provide big integer and decimal support.

You will also require a CPU with the AES-NI instruction set.

**Installation instructions**

The software has only been tested on Linux and OSX.  We have not attempted to build in a Windows environment yet, but have not used any platform specific code and so a Windows build should hopefully be reasonably achievable.

To build the C++ examples and install the library files, run
~~~~
cd build/
cmake -DCMAKE_BUILD_TYPE=Release .. && make install
~~~~
The library files will be installed into the `$root/include/` directory and the executable to run rank and search examples will be installed into the `$root/bin`directory.

The library is header-only and can be used in your own projects by including the `$root/include/` directory (and including boost_multiprecision).

**Folder hierarchy**

* `bin/` -- will contain the examples executable
* `build/` -- used for out-of-source CMake builds
* `examples/` -- C++ exemplar rank and parallel search examples
* `include/` -- will contain the header-only library files
* `matlab/rank` -- contains a minimal implementation of the path count rank algorithm in Java.  This is primarily intended to allow users to run rank estimations directly from Matlab.
* `matlab/` -- example usage of running rank estimations directly from Matlab using the Java implementation.
* `src/` -- C++ library source
* `test/` -- C++ unit tests

**Using Matlab/Java implementation**

To use the Matlab/Java implementation, compile the Java source code and move the library to the matlab folder.
~~~~
cd matlab/rank/
mvn package
cp target/LabynkyrRankJ-1.0-jar-with-dependencies.jar ../
~~~~

Now you should be ready do run the matlab examples.
