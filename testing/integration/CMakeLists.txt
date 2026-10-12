##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#
# @note God will make sure evil gets punished.
#       Yeah? Then explain Europe to me.
#

cmake_minimum_required(VERSION 3.27)

# TODO add ctest support

project(
   IntegrationTesting
   VERSION     1.0.0
   DESCRIPTION "Builds integration shared libraries")

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
   -Wno-format-security
   -fmax-errors=1)

target_include_directories(YMRootIntLib INTERFACE
   ${YM_IntgTestDir}/common
   ${YM_IntgTestDir}/ym/common
   ${YM_ProjRootDir}/ym/common)

if (YM_CovEnabled)
   message("Compiling for coverage...")
   file(MAKE_DIRECTORY ${YM_IntgTestDir}/cov_build/profiles)
   set(YM_CovFlags -fprofile-instr-generate -fcoverage-mapping)
   target_compile_options(YMRootIntLib INTERFACE ${YM_CovFlags})
   target_link_options(   YMRootIntLib INTERFACE ${YM_CovFlags})
   unset(YM_CovFlags)
elseif (NOT YM_DEBUG)
   target_compile_options(YMRootIntLib INTERFACE -O2)
endif()

target_compile_definitions(YMRootIntLib INTERFACE YM_INTEGRATIONTEST=${YM_INTEGRATIONTEST})
target_compile_definitions(YMRootIntLib INTERFACE YM_PRINT_TO_SCREEN=${YM_PRINT_TO_SCREEN})
target_compile_definitions(YMRootIntLib INTERFACE YM_DEBUG=${YM_DEBUG})
target_compile_definitions(YMRootIntLib INTERFACE YM_LITE=${YM_LITE})
target_compile_definitions(YMRootIntLib INTERFACE YM_USE_HEAP_AS_FALLBACK=${YM_USE_HEAP_AS_FALLBACK})
target_compile_definitions(YMRootIntLib INTERFACE YM_EXCEPTIONS_ENABLED=${YM_EXCEPTIONS_ENABLED})

if (YM_DEBUG)
   target_compile_options(YMRootIntLib INTERFACE -g)
   target_link_libraries(YMRootIntLib INTERFACE stdc++exp)
endif()

include(${YM_ExtLibsDir}/targets.cmake)
cmake_language(CALL link-to-fmt YMRootIntLib)

target_link_directories(YMRootIntLib INTERFACE ${YM_CustomLibsDir})

add_custom_target(clean-all
   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
   COMMAND rm -rf build/)

include(${YM_IntgTestDir}/common)
cmake_language(CALL intgbuild-intg.common ${YM_Ctx_JSON})
target_link_libraries(YMRootIntLib INTERFACE intg.common)

set(YM_TopLevelSubBuilds file_handling)
foreach(SubBuild ${YM_TopLevelSubBuilds})
   include(${YM_IntgTestDir}/${SubBuild}/build.cmake)
   cmake_language(CALL intgbuild-${SubBuild} ${YM_Ctx_JSON})
   # target "all" implicitly set to build all targets - no need to explicitly add dependency
endforeach()
