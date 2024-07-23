##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

## ut.common
#
# @brief Defines target to build the unittest framework.
#
function(ut.common)
   set(Target ${CMAKE_CURRENT_FUNCTION})

   add_library(${Target} SHARED)

   target_sources(${Target} PRIVATE
      ${CMAKE_SOURCE_DIR}/common/datashuttle.cpp
      ${CMAKE_SOURCE_DIR}/common/nameable.cpp
      ${CMAKE_SOURCE_DIR}/common/testcase.cpp
      ${CMAKE_SOURCE_DIR}/common/testsuitebase.cpp
      ${CMAKE_SOURCE_DIR}/common/utception.cpp
   )

   set_target_properties(${Target} PROPERTIES VERSION ${PROJECT_VERSION})
   set_target_properties(${Target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_UTLibDir})
endfunction()
