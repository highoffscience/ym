##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

##
# @brief Defines target to build integration test utilities.
#
# @param Ctx_JSON -- Context object.
#
function(intgbuild-intg.common Ctx_JSON)

   string(REGEX REPLACE "^[^-]+-" ""  BaseBuild    ${CMAKE_CURRENT_FUNCTION})
   string(      REPLACE "."       "/" BaseBuildDir ${BaseBuild})

   set(Target ${BaseBuild})
   add_library(${Target} SHARED)

   set(Srcs
      datashuttle.cpp
      testsuitebase.cpp)
   list(TRANSFORM Srcs PREPEND ${YM_IntgTestDir}/common/)
   target_sources(${Target} PRIVATE ${Srcs})

   target_sources(${Target} PRIVATE ${YM_IntgTestDir}/${BaseBuildDir}/testsuite.cpp)

   set_target_properties(${Target} PROPERTIES VERSION ${PROJECT_VERSION})
   set_target_properties(${Target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_CustomLibsDir})
   set_target_properties(${Target} PROPERTIES
      CXX_STANDARD 20 # maximum standard allowed by cppyy which uses these files
      CXX_STANDARD_REQUIRED ON)

   cmake_language(CALL ym-link-to-fmt ${Target})

endfunction()
