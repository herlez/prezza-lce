cmake_minimum_required (VERSION 2.6)
project (prezza-lce)

# configure a header file to pass some of the CMake settings
# to the source code
  
set(CMAKE_CXX_FLAGS "-Wall -Werror -O3")  
  
add_executable(timeTest ./src/testTime.cpp)
add_executable(timeCorrectness ./src/testCorrectness.cpp)
add_executable(sss ./src/sparseSuffixSorting.cpp)
