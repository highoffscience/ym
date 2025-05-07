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

   add_custom_target(${TargetAll})
   add_custom_target(${TargetRun})
   add_library(${TargetInt} INTERFACE)

   target_link_libraries(${TargetInt} INTERFACE ym-interface)

   include(${ProjRootDir}/ym/common/build.cmake)
   cmake_language(CALL srcbuild-${BaseBuild} ${Ctx_JSON})
   target_link_libraries(${TargetInt} INTERFACE ${BaseBuild})

   add_custom_command(TARGET ${TargetInt}
      POST_BUILD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      BYPRODUCTS        ${CMAKE_SOURCE_DIR}/covbuild/profiles/
      COMMAND ${CMAKE_COMMAND} -D COV_ENABLED=${YM_CovEnabled}
                               -D TARGET_NAME=${SubTarget}
                               -D OBJECT_NAME=lib${Target}.so
                               -P ${CMAKE_SOURCE_DIR}/run_unittest.cmake)

   set(SubBuilds logger textlogger timer ymassert ymdefs ymutils)
   foreach(SubBuild ${SubBuilds})

      set(SubBaseBuild ${Build}.${SubBuild})
      set(SubTarget    ${Build}.${SubBuild}-unittest)
      set(SubTargetRun ${Build}.${SubBuild}-run)

      set(SubBuildPath ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${SubBuild})

      if(EXISTS  ${SubBuildPath}/build.cmake)
         include(${SubBuildPath}/build.cmake)
         cmake_language(CALL utbuild-${SubBaseBuild} Ctx_JSON)
      else()
         add_library(${SubTarget} SHARED)
         target_sources(${SubTarget} PRIVATE ${SubBuildPath}/testsuite.cpp)
         target_link_libraries(${SubTarget} PRIVATE ${TargetInt})
      endif()

      add_dependencies(${TargetAll} ${SubTarget})
      add_dependencies(${TargetRun} ${SubTargetRun})
      add_custom_target(${SubTargetRun} DEPENDS ${SubTarget})

      add_dependencies(${SubTargetRun} check-venv)

      add_custom_command(TARGET ${SubTargetRun}
         POST_BUILD
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
         BYPRODUCTS        ${CMAKE_SOURCE_DIR}/covbuild/profiles
         COMMAND ${YM_Python} "run_unittest.py --targetnames=${SubBaseBuild} --libraryname=${BaseBuild}")

   endforeach()

   if (${YM_CovEnabled})
      add_custom_command(TARGET ${TargetRun}
         POST_BUILD
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
         BYPRODUCTS        ${CMAKE_SOURCE_DIR}/covbuild/profiles/
         COMMAND ${CMAKE_COMMAND} -D TARGET_NAME=${Target}
                                  -D SUBTARGET_NAMES="${SubBuilds}"
                                  -D OBJECT_NAME=lib${Target}.so
                                  -P ${CMAKE_SOURCE_DIR}/merge_covdata.cmake)
   endif()

endfunction()
