##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#
# @note God will make sure evil gets punished.
#       Yeah? Then explain Europe to me.
#

cmake_minimum_required(VERSION 3.27)

if (${YM_COV_ENABLED})
   message("Compiling for coverage...")
endif()

project(
   UnitTest
   VERSION     1.0.0
   DESCRIPTION "Builds unittest shared libraries and optionally runs them")

set(YM_Ctx_JSON    "{}")
set(YM_ProjRootDir ${CMAKE_SOURCE_DIR}/../..)
set(YM_UTLibDir    ${CMAKE_BINARY_DIR}/customlibs)
set(YM_ExtLibsDir  ${YM_ProjRootDir}/ym/extlibs)

string(JSON Ctx_JSON SET ${Ctx_JSON} "ProjRootDir" \"${YM_ProjRootDir}\")
string(JSON Ctx_JSON SET ${Ctx_JSON} "UTLibDir"    \"${YM_UTLibDir}\")
string(JSON Ctx_JSON SET ${Ctx_JSON} "ExtLibsDir"  \"${YM_ExtLibsDir}\")

add_library(YMRootIntLib INTERFACE)

target_compile_options(YMRootIntLib INTERFACE
   -Werror
   -Wall
   -Wextra
   -Wno-format-security)

target_compile_definitions(YMRootIntLib INTERFACE
   YM_PRINT_TO_SCREEN
   FMT_HEADER_ONLY)

add_subdirectory(${YM_ExtLibsDir} ${YM_ExtLibsDir}/build)
target_link_libraries(YMRootIntLib fmt::fmt)
target_include_directories(YMRootIntLib INTERFACE ${YM_ExtLibsDir}/build/_deps/fmt-src/include)

set_target_properties(YMRootIntLib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(YMRootIntLib PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_UTLibDir})

target_link_directories(YMRootIntLib PRIVATE ${YM_UTLibDir})

include(common/build.cmake)
cmake_language(CALL ut.common ${Ctx_JSON})
# TODO add_subdirectory
target_link_libraries(${TargetInt} INTERFACE ut.common)

include(ym/build.cmake)
cmake_language(CALL ym ${Ctx_JSON})
