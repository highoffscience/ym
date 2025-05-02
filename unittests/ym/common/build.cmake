##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

## utbuild-ym.common
#
# @brief Defines target to build ym.common, and all child unittests.
#
# @param Ctx_JSON -- Context object.
#
# @note Library directory (see README for description).
#
function(utbuild-ym.common Ctx_JSON)

   set(TargetAll ym.common-unittests)
   set(TargetRun ym.common-run)
   set(TargetInt ym.common-interface)

   add_custom_target(${TargetAll})
   add_custom_target(${TargetRun})
   add_library(${TargetInt} INTERFACE)

   target_link_libraries(TargetInt INTERFACE ym-interface)

   include(${ProjRootDir}/ym/common/build.cmake)
   cmake_language(CALL srcbuild-ym.common ${Ctx_JSON})
   target_link_libraries(${TargetInt} INTERFACE ym.common)

   set(SubTargets logger textlogger timer ymassert ymdefs ymutils)
   foreach(SubTarget ${SubTargets})
      include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${SubTarget}/build.cmake)
      cmake_language(CALL utbuild-ym.common.${SubTarget} ${Ctx_JSON})
   endforeach()

   foreach(SrcFileName ${SrcFileNames})
      if(EXISTS                           ${SrcFilesPath}/${SrcFileName}.cpp)
         target_sources(${Target} PRIVATE ${SrcFilesPath}/${SrcFileName}.cpp)
      endif()

      set(SubTarget    ${Target}.${SubBuild})
      set(SubTargetAll ${SubTarget}-unittest)
      set(SubTargetRun ${SubTarget}-run)

      set(SubBuildUTPath ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${SubBuild})

      if(EXISTS  ${SubBuildUTPath}/build.cmake)
         include(${SubBuildUTPath}/build.cmake)
         cmake_language(CALL ${SubTarget} Ctx_JSON)
      else()
         add_library(${SubTarget} SHARED)

         target_sources(${SubTarget} PRIVATE ${SubBuildUTPath}/testsuite.cpp)

         set_target_properties(${SubTarget} PROPERTIES OUTPUT_NAME ${SubTarget}-unittest)
      endif()

      add_dependencies(${TargetAll} ${SubTarget})
      add_dependencies(${TargetRun} ${SubTargetRun})

      add_custom_target(${SubTargetRun} DEPENDS ${SubTarget})

      add_custom_command(TARGET ${SubTargetRun}
         PRE_BUILD
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
         COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/check_pyvirtualenv.cmake)

      add_custom_command(TARGET ${SubTargetRun}
         POST_BUILD
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
         BYPRODUCTS        ${CMAKE_SOURCE_DIR}/covbuild/profiles/
         COMMAND ${CMAKE_COMMAND} -D COV_ENABLED=${COV_ENABLED}
                                  -D TARGET_NAME=${SubTarget}
                                  -D OBJECT_NAME=lib${Target}.so
                                  -P ${CMAKE_SOURCE_DIR}/run_unittest.cmake)
   endforeach()

   if (${COV_ENABLED})
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
