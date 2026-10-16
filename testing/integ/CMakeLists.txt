##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#
# God will make sure evil gets punished.
# Yeah? Then explain Europe to me.
#

cmake_minimum_required(VERSION 3.27...4.4.4) # 4-est version

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
   ${YM_IntegTestDir}/common
   ${YM_IntegTestDir}/ym/common
   ${YM_ProjRootDir}/ym/common)

if (YM_CovEnabled)
   message("Compiling for coverage...")
   file(MAKE_DIRECTORY ${YM_IntegTestDir}/cov_build/profiles)
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
cmake_language(CALL ym-link-to-fmt YMRootIntLib)

target_link_directories(YMRootIntLib INTERFACE ${YM_CustomLibsDir})

add_custom_target(clean-all
   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
   COMMAND rm -rf build/)

include(${YM_IntegTestDir}/common/build.cmake)
cmake_language(CALL ym-integbuild-integ.common ${YM_Ctx_JSON})
target_link_libraries(YMRootIntLib INTERFACE integ.common)

add_custom_target(integ-tests) # aka TargetAll
set(YM_TopLevelSubBuilds core)
foreach(SubBuild ${YM_TopLevelSubBuilds})
   include(${YM_IntegTestDir}/${SubBuild}/build.cmake)
   cmake_language(CALL ym-build-integ.${SubBuild} ${YM_Ctx_JSON})
   add_dependencies(integ-tests integ.${SubBuild}-tests)
endforeach()
