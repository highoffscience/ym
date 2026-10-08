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

set(CMAKE_CXX_STANDARD 20) # maximum standard allowed by cppyy
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(unit.common SHARED)

set(Srcs
   datashuttle.cpp
   testsuitebase.cpp)
list(TRANSFORM Srcs PREPEND ${YM_UnitTestDir}/common/)
target_sources(unit.common PRIVATE ${Srcs})
unset(Srcs)

target_include_directories(unit.common PRIVATE ${YM_ProjRootDir}/ym/common)

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

##
# @brief Defines target to build integration test utilities.
#
# @param Ctx_JSON -- Context object.
#
function(intgbuild-intg.common Ctx_JSON)

   string(REGEX REPLACE "^[^-]+-" ""  BaseBuild    ${CMAKE_CURRENT_FUNCTION})
   string(      REPLACE "."       "/" BaseBuildDir ${BaseBuild})

   set(Target ${BaseBuild})
   add_library(${Target} SHARED)

   set(Srcs
      datashuttle.cpp
      testsuitebase.cpp)
   list(TRANSFORM Srcs PREPEND ${YM_UnitTestDir}/common/)
   target_sources(${Target} PRIVATE ${Srcs})

   target_sources(${Target} PRIVATE ${YM_UnitTestDir}/${BaseBuildDir}/testsuite.cpp)

   set_target_properties(${Target} PROPERTIES VERSION ${PROJECT_VERSION})
   set_target_properties(${Target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_CustomLibsDir})

endfunction()
