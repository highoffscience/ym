##
# AUTO-GENERATED
#
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.16)

project(UnitTest VERSION 1.0.0 DESCRIPTION "Builds unittest shared libraries")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(RootDir           ${CMAKE_CURRENT_SOURCE_DIR}/../)
set(CommonDir         ${RootDir}/common/)
set(UnitTestDir       ${RootDir}/unittests/)
set(CommonUnitTestDir ${UnitTestDir}/common/)

add_library(random SHARED)

target_sources(random PRIVATE
   ${CommonDir}/random.cpp
   ${UnitTestDir}/unittestbase.cpp
   ${CommonUnitTestDir}/random/random_ut.cpp)

set_target_properties(random PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(random PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/ym/common/)

target_compile_options(random PRIVATE -Werror -Wall -Wextra -Ofast)

target_include_directories(random PRIVATE ${CommonDir})
target_include_directories(random PRIVATE ${UnitTestDir})
target_include_directories(random PRIVATE ${CommonUnitTestDir})
