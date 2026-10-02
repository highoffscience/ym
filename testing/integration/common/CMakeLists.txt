##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

project(
   UnitTestCommon
   VERSION     1.0.0
   DESCRIPTION "Builds common unittest framework and communication functionalities")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(unit.common SHARED)

set(Srcs
   datashuttle.cpp
   testsuitebase.cpp)
list(TRANSFORM Srcs PREPEND ${YM_UnitTestDir}/common/)
target_sources(unit.common PRIVATE ${Srcs})
unset(Srcs)

target_include_directories(unit.common PRIVATE ${YM_ProjRootDir}/ym/common/)

find_library(YM_FMT_LIB
   NAMES fmt
   PATHS ${YM_ExtLibsDir}/build/fmt)

if (YM_FMT_LIB)
   target_link_libraries(unit.common PRIVATE ${YM_FMT_LIB})
   target_include_directories(unit.common PRIVATE ${YM_ExtLibsDir}/fmt/include)
else()
   message(FATAL_ERROR "fmt lib not found. Must build external libraries first.")
endif()

set_target_properties(unit.common PROPERTIES
   VERSION ${PROJECT_VERSION}
   LIBRARY_OUTPUT_DIRECTORY ${YM_CustomLibsDir})
