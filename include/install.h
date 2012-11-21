/*! \page install Installing BayesOpt
\tableofcontents

Bayesian-optimization uses standard C/C++ code and it can be
compiled in different platforms and linked with different APIs.

\section instc Install C/C++ library:

The easiest way to compile this library is using the cross platform
and cross compiler tool <a href="http://www.cmake.org/">CMake</a>.

This code uses Boost libraries for matrix operations (uBlas), random
number generation, math functions and smart pointer. They can be found
in many Linux and MacOS repositories. It can also be downloaded from
http://www.boost.org.

\subsection cinlinux Linux:
For Ubuntu/Debian, the minimum dependencies can be optained by running:
\verbatim
>> sudo apt-get install libboost-dev cmake g++
\endverbatim

And for all dependencies:
\verbatim
>> sudo apt-get install libboost-dev python-dev python-numpy cmake g++ cython octave-headers
\endverbatim

\subsection cinmac MacOS:
There are many options. For example, you can install macports and run:
\verbatim
>> sudo port install boost gcc46 cmake
\endverbatim

Again, for full dependencies:
\verbatim
>> sudo port install boost python27 py27-numpy gcc46 cmake py27-cython octave
\endverbatim

\subsection compile Compile in Linux or MacOS:
In order to compile the source code in a *nix system, run this from a terminal.
\verbatim
>> cmake . 
>> make
>> sudo make install
\endverbatim

If you use \em ccmake instead of \em cmake you will access a graphical
interface to select features such as debug/release mode and if you
want to use shared libraries or not. Shared libraries are required to
run the Python interface.

If you have doxygen installed on your computer, you can compile the
documentation right after compiling the code by running.
\verbatim
>> make doc
\endverbatim
Thid documentation will appear in the "doc" subdirectory.

\subsection cinwin Windows and other systems:
Install this components:
\li CMake: http://www.cmake.org
\li Boost: http://www.boost.org
\li MinGW: http://www.mingw.org

MinGW can be replaced by your favorite C++ compiler (Visual Studio,
Intel, etc.).

Since Boost they are pure template libraries, they do not require
compilation. Just make sure the headers are on the include path. You
can also add an entry named BOOST_ROOT in CMake with the corresponding
path to the library.

<HR>

\section instpython Install Python interface:

Both Python and Numpy are needed if you want the python interface. The
library has been tested with Python 2.6 and 2.7. The interface relies
on Numpy arrays. Python development files such as Python.h are needed
to compile the interface.

In \b Ubuntu/Debian, you can get the dependencies by running:
\verbatim
>> sudo apt-get install python-dev python-numpy cython
\endverbatim

In \b MacOS you can install macports and run:
\verbatim
>> sudo port install python27 py27-numpy py27-cython
\endverbatim

It you want to \b modify the Python interface, you need to modify the
pyx file and run the Cython compiler.

\verbatim
$ cython --cplus bayesopt.pyx
\endverbatim

\b Important: Python requires bayesopt to be a \b shared library.

In \b Windows you might need to download and install:
\li Python: http://www.python.org
\li Numpy: http://new.scipy.org/download.html 
\li Cython: http://cython.org 

Make sure that CMake finds all the dependencies.

<HR>

\section instmatlab Install MATLAB/Octave interface

Make sure the library is compiled with the MATLAB_COMPATIBLE option.

Compile the interface using the compile_matlab.m or compile_octave.m
script, which can be found in the \em /matlab/ directory.

If bayesopt or nlopt are compiled as \b shared libraries, then, at run
time, MATLAB/Octave also needs to access to the libraries. For
example, in Linux and Mac OS you make sure to execute the
exportlocalpath.sh script is executed before calling MATLAB.

 */