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

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Context objects are useful for passing state around
set(YM_Ctx_JSON "{}")

add_library(YMRootIntLib INTERFACE)

target_compile_options(YMRootIntLib INTERFACE
   -Werror
   -Wall
   -Wextra
   -Wno-format-security)

target_include_directories(YMRootIntLib INTERFACE
   ${YM_UnitTestDir}/common
   ${YM_UnitTestDir}/ym/common
   ${YM_ProjRootDir}/ym/common)

set(YM_UTPYTHONPATHS ${YM_ProjRootDir}/ym:${YM_UnitTestDir})
if ($ENV{PYTHONPATH})
   set(ENV{PYTHONPATH} ${YM_UTPYTHONPATHS}:$ENV{PYTHONPATH})
else()
   set(ENV{PYTHONPATH} ${YM_UTPYTHONPATHS})
endif()
unset(YM_UTPYTHONPATHS)

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

if (${YM_DEBUG})
   target_compile_definitions(YMRootIntLib INTERFACE YM_DEBUG=1)
endif()

if (${YM_ENABLE_EXCEPTIONS})
   target_compile_definitions(YMRootIntLib INTERFACE YM_YES_EXCEPTIONS=1)
   target_compile_definitions(YMRootIntLib INTERFACE YM_NO_EXCEPTIONS=0)
else()
   target_compile_definitions(YMRootIntLib INTERFACE YM_YES_EXCEPTIONS=0)
   target_compile_definitions(YMRootIntLib INTERFACE YM_NO_EXCEPTIONS=1)
endif()

find_library(YM_FMT_LIB
   NAMES fmt
   PATHS ${YM_ExtLibsDir}/build/_deps/fmt-build)

if (YM_FMT_LIB)
   target_link_libraries(YMRootIntLib INTERFACE ${YM_FMT_LIB})
   target_include_directories(YMRootIntLib INTERFACE ${YM_ExtLibsDir}/build/_deps/fmt-src/include)
else()
   message(FATAL_ERROR "fmt lib not found. Must build external libraries first.")
endif()

target_link_directories(YMRootIntLib INTERFACE ${YM_CustomLibsDir})

add_custom_target(check-venv)
add_custom_command(TARGET check-venv # specialized command to run at build (run) time, not configure time
   PRE_BUILD
   WORKING_DIRECTORY ${YM_UnitTestDir}
   COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/check_venv.cmake)

add_custom_target(distclean
   WORKING_DIRECTORY ${YM_UnitTestDir}
   COMMAND rm -rf build/*)

add_subdirectory(${YM_UnitTestDir}/common)
target_link_libraries(YMRootIntLib INTERFACE unit.common)

set(YM_TopLevelSubBuilds ym)
foreach(SubBuild ${YM_TopLevelSubBuilds})
   include(${YM_UnitTestDir}/${SubBuild}/build.cmake)
   cmake_language(CALL unitbuild-${SubBuild} ${YM_Ctx_JSON})
endforeach()
