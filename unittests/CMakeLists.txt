##
# @author Forrest Jablonski
#

cmake_minimum_required(VERSION 3.16)

project(UnitTest VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(UnitTest unittest.cpp)

set_target_properties(UnitTest PROPERTIES OUTPUT_NAME unittest)

target_compile_options(UnitTest PRIVATE -Werror -Wall -Wextra -O2)

set(CommonDir ${CMAKE_CURRENT_SOURCE_DIR}/../common/)
set(CommonUnitTestDir ${CMAKE_CURRENT_SOURCE_DIR}/common/)

target_sources(UnitTest PUBLIC
   ${CommonDir}/random.cpp
   ${CommonUnitTestDir}/random_unittest.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/unittestbase.cpp)

target_include_directories(UnitTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(UnitTest PUBLIC ${CommonDir})
target_include_directories(UnitTest PUBLIC ${CommonUnitTestDir})
