##
# @file    cov_toolchain.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_C_COMPILER   /usr/bin/clang)
set(CMAKE_ASM_COMPILER /usr/bin/clang)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(YM_CovFlags -fprofile-instr-generate -fcoverage-mapping)
list(APPEND CMAKE_CXX_FLAGS           ${YM_CovFlags})
list(APPEND CMAKE_C_FLAGS             ${YM_CovFlags})
list(APPEND CMAKE_SHARED_LINKER_FLAGS ${YM_CovFlags})
