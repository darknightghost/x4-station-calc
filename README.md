# X4:Foundnations Station Editor

### How to complie

#### Build requirements

* Qt5(Environment variables `QT` and `Path` must be setted on windows)
* cmake
* gcc/mingw/visual studio
* doxygen(Optional)


#### Compile

* Code documents can be found in `doc/` and executable files can be found in `bin/`.

##### Windows
1. `md build`
1. `cd build`
1. `cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release ..`(You should change the  -G option to the version of the Vsiual Studio on your computer).
1. `cmake .`
1. `cmake --build .`

##### Linux
1. `mkdir build`
1. `cd build`
1. `cmake -DCMAKE_BUILD_TYPE=Release ..`
1. `cmake --build .`
