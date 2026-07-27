##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

project(
   Ym-Doxy
   VERSION     1.0.0
   DESCRIPTION "Generates compile_commands.json for doxygen to consume")

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(YMDoxyDummy INTERFACE)

target_include_directories(YMDoxyDummy INTERFACE
   ${CMAKE_CURRENT_SOURCE_DIR}/common)

target_compile_definitions(YMDoxyDummy INTERFACE YM_UNITTEST=0)
target_compile_definitions(YMDoxyDummy INTERFACE YM_PRINT_TO_SCREEN=0)
target_compile_definitions(YMDoxyDummy INTERFACE YM_DEBUG=0)
target_compile_definitions(YMDoxyDummy INTERFACE YM_LITE=0)
target_compile_definitions(YMDoxyDummy INTERFACE YM_USE_HEAP_AS_FALLBACK=1)
target_compile_definitions(YMDoxyDummy INTERFACE YM_YES_EXCEPTIONS=1)
target_compile_definitions(YMDoxyDummy INTERFACE YM_NO_EXCEPTIONS=0)

find_library(YM_FMT_LIB
   NAMES fmt
   PATHS ${YM_ExtLibsDir}/build/fmt)

if (YM_FMT_LIB)
   target_link_libraries(YMDoxyDummy INTERFACE ${YM_FMT_LIB})
   target_include_directories(YMDoxyDummy INTERFACE ${YM_ExtLibsDir}/fmt/include)
else()
   message(FATAL_ERROR "fmt lib not found. Must build external libraries first.")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/common/build.cmake)
cmake_language(CALL srcbuild-ym.common "")
target_link_libraries(ym.common PRIVATE YMDoxyDummy)

find_package(Doxygen)

if(DOXYGEN_FOUND)
   set(DOXYGEN_IN  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
   set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

   configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

   add_custom_target(docs
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM)
else()
   message(WARNING "Doxygen not found. 'doc' target not available.")
endif()

add_custom_target(dist-clean
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   COMMAND rm -rf build/)
