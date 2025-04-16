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
   message("Cov mode enabled")
endif()

project(
   UnitTest
   VERSION     1.0.0
   DESCRIPTION "Builds unittest shared libraries and optionally runs them")

set(YM_Ctx_JSON    "{}")
set(YM_ProjRootDir ${CMAKE_SOURCE_DIR}/../..)
set(YM_UTLibDir    ${CMAKE_BINARY_DIR}/customlibs)

string(JSON Ctx_JSON SET ${Ctx_JSON} "ProjRootDir" \"${YM_ProjRootDir}\")
string(JSON Ctx_JSON SET ${Ctx_JSON} "UTLibDir"    \"${YM_UTLibDir}\")

add_library(YMRootIntLib INTERFACE)

target_compile_options(YMRootIntLib INTERFACE
   -Werror
   -Wall
   -Wextra
   -Wno-format-security)

target_compile_definitions(YMRootIntLib INTERFACE
   YM_PRINT_TO_SCREEN
   FMT_HEADER_ONLY)

include(FetchContent)

set(BUILD_SHARED_LIBS TRUE)
FetchContent_Declare(
   fmt
   GIT_REPOSITORY https://github.com/fmtlib/fmt
   GIT_TAG        8303d140a1a11f19b982a9f664bbe59a1ccda3f4) # 11.1.2
FetchContent_MakeAvailable(fmt)

target_link_libraries(YMRootIntLib fmt::fmt)

target_include_directories(YMRootIntLib INTERFACE ${YM_ProjRootDir}/ym/fmt/include/)

set_target_properties(YMRootIntLib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(YMRootIntLib PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_UTLibDir})

# target_link_directories(YMRootIntLib PRIVATE ${UTLibDir})

## END ##

include(common/build.cmake)
cmake_language(CALL ut.common ${Ctx_JSON})

include(ym/build.cmake)
cmake_language(CALL ym ${Ctx_JSON})
