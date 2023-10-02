##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

set(Target ut-common)

add_library(${Target} SHARED)

target_sources(${Target} PRIVATE
   ${CMAKE_SOURCE_DIR}/common/datashuttle.cpp
   ${CMAKE_SOURCE_DIR}/common/nameable.cpp
   ${CMAKE_SOURCE_DIR}/common/testcase.cpp
   ${CMAKE_SOURCE_DIR}/common/testsuitebase.cpp
   ${CMAKE_SOURCE_DIR}/common/utception.cpp
)

target_include_directories(${Target} PRIVATE ${RootDir}/ym/fmt/include/)

set_target_properties(${Target} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${Target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${UtLibDir})

set(CompileFlags
   -Werror
   -Wall
   -Wextra
   -Wno-format-security
   -O2
   -DYM_UT_DBG
   -DYM_PRINT_TO_SCREEN)

target_compile_options(${Target} PRIVATE ${CompileFlags} -DFMT_HEADER_ONLY)

unset(CompileFlags)
unset(Target      )
