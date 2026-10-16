##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

##
# @brief Defines target to build specified integration tests.
#
# @param Ctx_JSON -- Context object.
#
function(ym-build-integ.core.file_handling Ctx_JSON)

   string(REGEX REPLACE "^[^-]+-" ""  BaseBuild    ${CMAKE_CURRENT_FUNCTION})
   string(      REPLACE "."       "/" BaseBuildDir ${BaseBuild})

   set(Target ${BaseBuild}-tests)
   add_library(${Target} SHARED)

   target_link_libraries(${Target} PRIVATE integ.core-interface)

   target_sources(${Target} PRIVATE ${YM_IntegTestDir}/${BaseBuildDir}/testsuite.cpp)

   # TODO custom libraries and exes don't always go to their desired output directory

   set_target_properties(${Target} PROPERTIES VERSION ${PROJECT_VERSION})
   set_target_properties(${Target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_CustomLibsDir})

endfunction()
