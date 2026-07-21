##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 4.0)

project(
   Ym
   VERSION     1.0.0
   DESCRIPTION "Top level build commands"
   LANGUAGES NONE) # delete when more commands get added

find_package(Doxygen)

if(DOXYGEN_FOUND)
   set(DOXYGEN_IN  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
   set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

   configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

   add_custom_target(doc
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM)
else()
   message(WARNING "Doxygen not found. 'doc' target not available.")
endif()
