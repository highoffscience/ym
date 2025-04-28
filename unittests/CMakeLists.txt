##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#
# @note God will make sure evil gets punished.
#       Yeah? Then explain Europe to me.
#

cmake_minimum_required(VERSION 3.27)

project(
   UnitTest
   VERSION     1.0.0
   DESCRIPTION "Builds unittest shared libraries and runs them")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Context objects may be useful for passing state around
set(YM_Ctx_JSON "{}")

add_library(YMRootIntLib INTERFACE)

target_compile_options(YMRootIntLib INTERFACE
   -Werror
   -Wall
   -Wextra
   -Wno-format-security)

if (${YM_CovEnabled})
   message("Compiling for coverage...")
   set(YM_CovFlags -fprofile-instr-generate -fcoverage-mapping)
   target_compile_options(YMRootIntLib INTERFACE ${YM_CovFlags})
   target_link_options(   YMRootIntLib INTERFACE ${YM_CovFlags})
   unset(YM_CovFlags)
else()
   target_compile_options(YMRootIntLib INTERFACE -O2)
endif()

if (${YM_PRINT_TO_SCREEN})
   target_compile_definitions(YMRootIntLib INTERFACE YM_PRINT_TO_SCREEN=1)
endif()

if (EXISTS ${YM_ExtLibsDir}/build/_deps/fmt-build/libfmt.so)
   target_link_directories(   YMRootIntLib INTERFACE ${YM_ExtLibsDir}/build/_deps/fmt-build)
   target_include_directories(YMRootIntLib INTERFACE ${YM_ExtLibsDir}/build/_deps/fmt-src/include)
   target_link_libraries(     YMRootIntLib INTERFACE fmt)
else()
   message(FATAL_ERROR "fmt lib not found. Must build external libraries first.")
endif()

set_target_properties(YMRootIntLib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(YMRootIntLib PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_CustomLibsDir})

target_link_directories(YMRootIntLib INTERFACE ${YM_CustomLibsDir})

add_subdirectory(${CMAKE_SOURCE_DIR}/common)
target_link_libraries(YMRootIntLib INTERFACE ut.common)

include(ym/build.cmake)
cmake_language(CALL ym ${Ctx_JSON})
