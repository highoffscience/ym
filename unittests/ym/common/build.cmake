##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

## utbuild-ym.common
#
# @brief Defines target to build all child unittests.
#
# @param Ctx_JSON -- Context object.
#
# @note Library directory (see README for description).
#
function(utbuild-ym.common Ctx_JSON)

   set(BaseBuild ym.common)
   set(TargetAll ${BaseBuild}-unittests)
   set(TargetRun ${BaseBuild}-run)
   set(TargetInt ${BaseBuild}-interface)

   string(REPLACE "." "/" BaseBuildDir ${BaseBuild})

   add_custom_target(${TargetAll})
   add_custom_target(${TargetRun})
   add_library(${TargetInt} INTERFACE)

   target_link_libraries(${TargetInt} INTERFACE ym-interface)

   include(${YM_ProjRootDir}/${BaseBuildDir}/build.cmake)
   cmake_language(CALL srcbuild-${BaseBuild} ${Ctx_JSON})
   target_link_libraries(${TargetInt} INTERFACE ${BaseBuild})
   target_link_libraries(${BaseBuild} PRIVATE ${TargetInt})
   set_target_properties(${BaseBuild} PROPERTIES VERSION ${PROJECT_VERSION})
   set_target_properties(${BaseBuild} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_CustomLibsDir})

   set(SubBuilds logger textlogger timer ymassert ymdefs ymutils)
   foreach(SubBuild ${SubBuilds})

      set(SubBaseBuild ${BaseBuild}.${SubBuild})
      set(SubTarget    ${BaseBuild}.${SubBuild}-unittest)
      set(SubTargetRun ${BaseBuild}.${SubBuild}-run)

      set(SubBuildDir ${YM_UnitTestDir}/${BaseBuildDir}/${SubBuild})

      if(EXISTS  ${SubBuildDir}/build.cmake)
         include(${SubBuildDir}/build.cmake)
         cmake_language(CALL utbuild-${SubBaseBuild} Ctx_JSON)
      else()
         add_library(${SubTarget} SHARED)
         target_sources(${SubTarget} PRIVATE ${SubBuildDir}/testsuite.cpp)
         target_link_libraries(${SubTarget} PRIVATE ${TargetInt})
         set_target_properties(${SubTarget} PROPERTIES VERSION ${PROJECT_VERSION})
         set_target_properties(${SubTarget} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_CustomLibsDir})
      endif()

      add_custom_target(${SubTargetRun} DEPENDS ${SubTarget})
      add_dependencies(${SubTargetRun} check-venv)
      add_dependencies(${TargetAll} ${SubTarget})
      add_dependencies(${TargetRun} ${SubTargetRun})

      add_custom_command(TARGET ${SubTargetRun}
         POST_BUILD
         WORKING_DIRECTORY ${YM_UnitTestDir}
         BYPRODUCTS        ${YM_UnitTestDir}/covbuild/profiles
         COMMAND ${CMAKE_COMMAND} -E env
            PYTHONPATH=$ENV{PYTHONPATH}
            ${YM_Python} run_unittest.py
               --unittestdir=${YM_UnitTestDir}
               --projrootdir=${YM_ProjRootDir}
               --builddir=${CMAKE_BINARY_DIR}
               --suitename=${SubBaseBuild}
               --libraryname=lib${BaseBuild}.so
               --covenabled=${YM_CovEnabled})

   endforeach()

   if (${YM_CovEnabled})
      add_custom_command(TARGET ${TargetRun}
         POST_BUILD
         WORKING_DIRECTORY ${YM_UnitTestDir}
         BYPRODUCTS        ${YM_UnitTestDir}/covbuild/profiles
         COMMAND ${YM_Python} merge_cov_profiles.py --binarydir=${CMAKE_BINARY_DIR} --libraryname=lib${BaseBuild}.so)
   endif()

endfunction()
