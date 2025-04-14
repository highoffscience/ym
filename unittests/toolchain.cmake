##
# @file    toolchain.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_C_COMPILER   /usr/bin/gcc)
set(CMAKE_ASM_COMPILER /usr/bin/gcc)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

list(APPEND CMAKE_CXX_FLAGS -O2)
