##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

##
# @brief Defines target to build specified unit test.
#
# @param Ctx_JSON -- Context object.
#
function(unitbuild-ym.common.datalogger Ctx_JSON)

   string(REGEX REPLACE "^[^-]+-" ""  BaseBuild    ${CMAKE_CURRENT_FUNCTION})
   string(      REPLACE "."       "/" BaseBuildDir ${BaseBuild})

   set(Target ${BaseBuild}-unittests)
   add_library(${Target} SHARED)

   target_link_libraries(${Target} PRIVATE ym.common-interface)

   target_sources(${Target} PRIVATE ${YM_UnitTestDir}/${BaseBuildDir}/testsuite.cpp)

   set_target_properties(${Target} PROPERTIES VERSION ${PROJECT_VERSION})
   set_target_properties(${Target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_CustomLibsDir})

endfunction()
