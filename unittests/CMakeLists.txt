##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.16)

project(UnitTest VERSION 1.0.0 DESCRIPTION "Builds unittest shared libraries")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-Ofast")

set(CommonDir         ${CMAKE_CURRENT_SOURCE_DIR}/../common/)
set(CommonUnitTestDir ${CMAKE_CURRENT_SOURCE_DIR}/common/)

add_library(random SHARED
            ${CommonDir}/random.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/unittestbase.cpp
            ${CommonUnitTestDir}/random/random_ut.cpp)

set_target_properties(random PROPERTIES VERSION ${PROJECT_VERSION})

# set_target_properties(random PROPERTIES PUBLIC_HEADER
#                       ${CommonDir}/random.h
#                       ${CMAKE_CURRENT_SOURCE_DIR}/unittestbase.h
#                       ${CommonUnitTestDir}/random/random_ut.h)

target_include_directories(random PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(random PRIVATE ${CommonDir})
target_include_directories(random PRIVATE ${CommonUnitTestDir})



# add_executable(UnitTest unittest.cpp)

# set_target_properties(UnitTest PROPERTIES OUTPUT_NAME unittest)

# target_compile_options(UnitTest PRIVATE -Werror -Wall -Wextra -O2)

# set(CommonDir ${CMAKE_CURRENT_SOURCE_DIR}/../common/)
# set(CommonUnitTestDir ${CMAKE_CURRENT_SOURCE_DIR}/common/)

# target_sources(UnitTest PUBLIC
#    ${CommonDir}/random.cpp
#    ${CommonUnitTestDir}/random/random_unittest.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/unittestbase.cpp)

# target_include_directories(UnitTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# target_include_directories(UnitTest PUBLIC ${CommonDir})
# target_include_directories(UnitTest PUBLIC ${CommonUnitTestDir})
