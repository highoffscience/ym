##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.16)

project(random VERSION 1.0.0 DESCRIPTION "Builds Random shared libraries")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(RootDir        ${CMAKE_CURRENT_SOURCE_DIR}/../../)
set(CommonDir      ${RootDir}/ym/common/)
set(UnitTestDir    ${RootDir}/ym/unittests/)
set(SUTDir         ${RootDir}/ym/common/)
set(SUTUnitTestDir ${UnitTestDir}/ym/common/random/)

set(FMTDir ${RootDir}/ym/fmt/)

add_library(random SHARED)

target_sources(random PRIVATE
   ${SUTDir}/random.cpp
   ${UnitTestDir}/common/testcase.cpp
   ${UnitTestDir}/common/datashuttle.cpp
   ${UnitTestDir}/common/testsuitebase.cpp
   ${UnitTestDir}/common/nameable.cpp
   ${SUTUnitTestDir}/testsuite.cpp)

# TODO investigate FetchContent command
#target_link_libraries(random ${FMTDir}/build/libfmt.a)

set_target_properties(random PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(random PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/ym/common)

target_compile_options(random PRIVATE -Werror -Wall -Wextra -O2) # -DFMT_HEADER_ONLY

target_include_directories(random PRIVATE ${FMTDir}/include/)
target_include_directories(random PRIVATE ${CommonDir})
target_include_directories(random PRIVATE ${UnitTestDir}/common/)
target_include_directories(random PRIVATE ${SUTDir})
target_include_directories(random PRIVATE ${SUTUnitTestDir})
