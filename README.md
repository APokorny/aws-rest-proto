AWS Protocol Library


Example usage
-------------

```C++
..
```


Dependencies
-------------
* Boost Asio
* Boost Beast
* Kavsir MPL
* fmtlib
* Niels Lohmann Json


How to build with CMake
-----------------------
Either Create an integration project with the dependencies above, or make sure all the dependencies are found via `find_package` or cmake config files.

If you want to build an integartion project, follow the guide from `Effective Moden CMake` by Daniel Pfeiffer:

```CMake
project(my-app CXX)
cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0048 NEW)
set(as_subproject kvasir_mpl aws-rest-proto fmt nlohmann_json span-lite)

macro(find_package)
    if(NOT ${ARGV0} IN_LIST as_subproject)
        _find_package(${ARGV})
    endif()
endmacro()

option(JSON_BuildTests "enables tests" OFF)
option(FMT_INSTALL "install fmt" ON)
set(FMT_DOC  OFF)
set(JSON_BuildTests OFF)

add_subdirectory(mpl)
add_subdirectory(fmt)
add_subdirectory(json)
add_subdirectory(aws-rest-proto)
add_subdirectory(my-app)
```
