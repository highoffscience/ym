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

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_CXX_EXTENSIONS)
   set(DOXYGEN_CXX_STANDARD_FLAG "-std=gnu++${CMAKE_CXX_STANDARD}")
else()
   set(DOXYGEN_CXX_STANDARD_FLAG "-std=c++${CMAKE_CXX_STANDARD}")
endif()

set(YM_UNITTEST             0)
set(YM_PRINT_TO_SCREEN      0)
set(YM_DEBUG                0)
set(YM_LITE                 0)
set(YM_USE_HEAP_AS_FALLBACK 1)
set(YM_YES_EXCEPTIONS       1)
set(YM_NO_EXCEPTIONS        0)

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
