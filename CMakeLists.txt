##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

project(
   Ym-Doxy
   VERSION     1.0.0
   DESCRIPTION "Generates doxygen docs"
   LANGUAGES   NONE)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER   clang)
set(CMAKE_ASM_COMPILER clang)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS True)

find_package(Doxygen)
if(DOXYGEN_FOUND)
   set(DOXYGEN_IN  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
   set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

   configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

   add_custom_target(docs
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM)
else()
   message(WARNING "Doxygen not found. 'docs' target not available.")
endif()

add_custom_target(dist-clean
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   COMMAND rm -rf build/)
