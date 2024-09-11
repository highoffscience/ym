##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

## ym.common
#
# @brief Defines target to build ym.common, and all child unittests.
#
function(ym.common)
   set(SubBuilds argparser datalogger fileio memio ops rng textlogger threadsafeproxy timer ymdefs ymerror)
   set(SrcFilesPath ${YM_ProjRootDir}/ym/common/)
   set(Target       ${CMAKE_CURRENT_FUNCTION})
   set(TargetAll    ${Target}_all)
   set(TargetRun    ${Target}_run)
   
   add_library(${Target} SHARED)

   add_custom_target(${TargetAll})
   add_custom_target(${TargetRun})

   target_sources(${Target} PRIVATE ${SrcFilesPath}/logger.cpp)

   set_target_properties(${Target} PROPERTIES VERSION ${PROJECT_VERSION})
   set_target_properties(${Target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_UTLibDir})

   target_compile_options(${Target} PRIVATE -DYM_COMMON_UT_DBG)

   if (${COV_ENABLED})
      target_compile_options(${Target} PRIVATE ${YM_CovFlags})
      target_link_options(   ${Target} PRIVATE ${YM_CovFlags})
   endif()

   foreach(SubBuild IN LISTS SubBuilds)
      if(EXISTS                           ${SrcFilesPath}/${SubBuild}.cpp)
         target_sources(${Target} PRIVATE ${SrcFilesPath}/${SubBuild}.cpp)
      endif()

      set(SubTarget    ym.common.${SubBuild})
      set(SubTargetRun ${SubTarget}_run)

      set(SubBuildUTPath ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${SubBuild}/)

      if(EXISTS  ${SubBuildUTPath}/build.cmake)
         include(${SubBuildUTPath}/build.cmake)
         cmake_language(CALL ym.common.${SubBuild})
      else()
         add_library(${SubTarget} SHARED)

         target_sources(${SubTarget} PRIVATE ${SubBuildUTPath}/testsuite.cpp)

         target_include_directories(${SubTarget} PRIVATE
            ${YM_ProjRootDir}/ym/common/
            ${CMAKE_SOURCE_DIR}/common/)

         target_link_directories(${SubTarget} PRIVATE ${YM_UTLibDir})

         target_link_libraries(${SubTarget} PRIVATE ut.common)
         target_link_libraries(${SubTarget} PRIVATE ym.common)

         set_target_properties(${SubTarget} PROPERTIES
            VERSION ${PROJECT_VERSION}
            LIBRARY_OUTPUT_DIRECTORY ${YM_UTLibDir}
            OUTPUT_NAME ${SubTarget}_unittest)
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
