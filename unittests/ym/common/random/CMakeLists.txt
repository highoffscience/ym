##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.16)

project(random VERSION 1.0.0 DESCRIPTION "Builds {args.SUT_name} shared libraries")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(RootDir        ${CMAKE_CURRENT_SOURCE_DIR}/../../)
set(CommonDir      ${RootDir}/ym/common/)
set(UnitTestDir    ${RootDir}/ym/unittests/)
set(SUTDir         ${RootDir}/ym/common/)
set(SUTUnitTestDir ${UnitTestDir}/ym/common/random/)

add_library(random SHARED)

target_sources(random PRIVATE
   ${SUTDir}/random.cpp
   ${UnitTestDir}/common/testcase.cpp
   ${UnitTestDir}/common/datashuttle.cpp
   ${UnitTestDir}/common/unittestbase.cpp
   ${UnitTestDir}/common/nameable.cpp
   ${SUTUnitTestDir}/testsuite.cpp)

set_target_properties(random PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(random PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${{CMAKE_BINARY_DIR}}/{args.filepath})

target_compile_options(random PRIVATE -Werror -Wall -Wextra -O2)

target_include_directories(random PRIVATE ${CommonDir})
target_include_directories(random PRIVATE ${UnitTestDir})
target_include_directories(random PRIVATE ${SUTDir})
target_include_directories(random PRIVATE ${SUTUnitTestDir})
