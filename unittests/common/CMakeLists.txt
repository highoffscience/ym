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

add_library(ut.common SHARED)

set(Srcs
   datashuttle.cpp
   nameable.cpp
   testcase.cpp
   testsuitebase.cpp
   utception.cpp)
list(TRANSFORM Srcs PREPEND ${YM_UnitTestDir}/common/)
target_sources(ut.common PRIVATE ${Srcs})
unset(Srcs)

if (${YM_UT_COMMON_DEBUG})
   target_compile_definitions(ut.common PRIVATE YM_DEBUG=1)
endif()

message(STATUS ">${YM_ExtLibsDir}/build/_deps/fmt-build<")

find_library(YM_FMT_LIB
   NAMES fmt
   PATHS ${YM_ExtLibsDir}/build/_deps/fmt-build)

message(STATUS ">${YM_FMT_LIB}<")

if (YM_FMT_LIB)
   target_link_libraries(ut.common PRIVATE ${YM_FMT_LIB})
else()
   message(FATAL_ERROR "fmt lib not found. Must build external libraries first.")
endif()

set_target_properties(ut.common PROPERTIES
   VERSION ${PROJECT_VERSION}
   LIBRARY_OUTPUT_DIRECTORY ${YM_CustomLibsDir})
