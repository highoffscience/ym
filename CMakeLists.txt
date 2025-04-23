##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

project(
   MyProject
   VERSION     1.0.0
   DESCRIPTION "Builds project tests")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(YM_Ctx_JSON      "{}")
set(YM_ProjRootDir   ${CMAKE_SOURCE_DIR}/..)
set(YM_CustomLibsDir ${CMAKE_BINARY_DIR}/customlibs)
set(YM_ExtLibsDir    ${YM_ProjRootDir}/ym/extlibs)
set(YM_CovFlags      -fprofile-instr-generate -fcoverage-mapping)

# Context objects may be useful for passing state around
string(JSON Ctx_JSON SET ${Ctx_JSON} "ProjRootDir"   \"${YM_ProjRootDir}\")
string(JSON Ctx_JSON SET ${Ctx_JSON} "CustomLibsDir" \"${YM_CustomLibsDir}\")
string(JSON Ctx_JSON SET ${Ctx_JSON} "ExtLibsDir"    \"${YM_ExtLibsDir}\")

add_library(YMRootIntLib INTERFACE)

target_compile_options(YMRootIntLib INTERFACE
   -Werror
   -Wall
   -Wextra
   -Wno-format-security)

if (${YM_COV_ENABLED})
   message("Compiling for coverage...")
   target_compile_options(YMRootIntLib INTERFACE ${YM_CovFlags})
   target_link_options(   YMRootIntLib INTERFACE ${YM_CovFlags})
else()
   target_compile_options(YMRootIntLib INTERFACE -O2)
endif()

if (${YM_PRINT_TO_SCREEN})
   target_compile_definitions(YMRootIntLib INTERFACE YM_PRINT_TO_SCREEN)
endif()

add_subdirectory(${YM_ExtLibsDir})

target_link_libraries(YMRootIntLib fmt::fmt)
target_include_directories(YMRootIntLib INTERFACE ${YM_ExtLibsDir}/build/_deps/fmt-src/include)

set_target_properties(YMRootIntLib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(YMRootIntLib PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_CustomLibsDir})

target_link_directories(YMRootIntLib INTERFACE ${YM_CustomLibsDir})

if (${YM_MAKE_UNITTESTS})
   add_subdirectory(${CMAKE_SOURCE_DIR}/unittests)
endif()
