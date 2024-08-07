##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#
# @todo Waiting for 64-bit TestU01
#

cmake_minimum_required(VERSION 3.27)

## ym.common.rng
#
# @brief Defines target to build the rng unittest.
#
function(ym.common.rng)
   set(Target    ${CMAKE_CURRENT_FUNCTION})
   set(TargetRun ${Target}_run)

   add_library(${Target} SHARED)

   add_custom_target(${TargetRun} DEPENDS ${Target})
   add_custom_command(TARGET ${TargetRun} POST_BUILD COMMAND python -m unittest ${Target}.testsuite)

   target_sources(${Target} PRIVATE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/testsuite.cpp)

   target_include_directories(${Target} PRIVATE
      ${YM_ProjRootDir}/ym/common/
      ${CMAKE_SOURCE_DIR}/common/)

   target_link_directories(${Target} PRIVATE ${YM_UTLibDir})

   target_link_libraries(${Target} PRIVATE ut.common)
   target_link_libraries(${Target} PRIVATE ym.common)

   set_target_properties(${Target} PROPERTIES VERSION ${PROJECT_VERSION})
   set_target_properties(${Target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_UTLibDir})
endfunction()
