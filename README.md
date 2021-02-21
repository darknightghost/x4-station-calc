# X4:Foundnations Station Editor

### How to complie

#### Build requirements

* Qt5(5.14.1 or above. Environment variables `QT` and `Path` must be setted on windows)
* OpenSSL(1.1.1 or above)
* cmake
* gcc/mingw/visual studio
* doxygen(Optional)


#### Compile

* Code documents can be found in `doc/` and executable files can be found in `bin/`.

##### Windows
1. `md build`
1. `cd build`
1. `cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release ..`(You should change the `-G` option to the version of the Vsiual Studio on your computer).
1. `cmake --build . --config Release`

##### Linux

###### How to Install Dependencies

1. Arch Linux
    1. Qt5
        1. `sudo pacman -S qt5`
    1. OpenSSL
        1. `sudo pacman -S openssl`
    1. cmake
        1. `sudo pacman -S cmake`
    1. gcc
        1. `sudo pacman -S gcc`
1. Ubuntu
    1. Qt5
        1. Download and install qt5 from <https://download.qt.io/archive/qt/>(the version must be greater than 5.14.0).
        1. Append the path of `gcc_64/lib` in your qt5 installalion path to environment variable `LD_LIBRARY_PATH`.
        1. Set the path of `gcc_64` in your qt5 installalion path to environment variable `QT`.
        1. Restart your computer.
        1. Replace the 3rd command in the build commands to `cmake -DCMAKE_PREFIX_PATH=${QT} -DCMAKE_BUILD_TYPE=Release ..`.
    1. OpenSSL
        1. `sudo apt-get install libssl-dev`
    1. cmake
        1. `sudo apt-get isntall cmake`
        1. `sudo apt-get isntall make`
    1. gcc
        1. `sudo apt-get isntall gcc`
        1. `sudo apt-get isntall g++`

###### Build Commands

1. `mkdir build`
1. `cd build`
1. `cmake -DCMAKE_BUILD_TYPE=Release ..`
1. `cmake --build .`
