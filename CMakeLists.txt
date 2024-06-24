cmake_minimum_required(VERSION 3.26)
project(Fem1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
#让链接器静态链接GCC和C++标准库

add_executable(Fem1 main.cpp)
