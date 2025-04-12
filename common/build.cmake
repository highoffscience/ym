##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

## ym.common
#
# @brief Defines target to build ym.common shared library.
#
# @param Ctx_JSON -- Context object.
#
function(ym.common Ctx_JSON)

   string(JSON ProjRootDir ERROR_VARIABLE ErrorOnGet GET ${Ctx_JSON} "ProjRootDir")
   if (ErrorOnGet)
      message(FATAL_ERROR ${ErrorOnGet})
   endif()

   string(JSON CovFlags ERROR_VARIABLE ErrorOnGet GET ${Ctx_JSON} "CovFlags")
   if (ErrorOnGet)
      message(FATAL_ERROR ${ErrorOnGet})
   endif()

   set(Srcs
      logger.cpp
      textlogger.cpp
      timer.cpp
      ymassert.cpp
      ymdefs.cpp
      ymutils.cpp)

   #TODO use CMAKE_CURRENT_FUNCTION_LIST_DIR to prepend Srcs

   set(Target       ${CMAKE_CURRENT_FUNCTION})
   set(TargetAll    ${Target}-unittests)
   set(TargetRun    ${Target}-run)

   add_library(${Target} PRIVATE)
   add_custom_target(${TargetAll})
   add_custom_target(${TargetRun})

   target_link_libraries(${Target} INTERFACE ym)

   if (YM_COMMON_DEBUG)
      target_compile_definitions(${Target} PRIVATE YM_COMMON_DEBUG)
   endif()

   if (${YM_COV_ENABLED})
      target_compile_options(${Target} PRIVATE ${CovFlags})
      target_link_options(   ${Target} PRIVATE ${CovFlags})
   endif()

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

         target_include_directories(${SubTarget} PRIVATE
            ${SrcFilesPath}
            ${CMAKE_SOURCE_DIR}/common/)

         target_link_directories(${SubTarget} PRIVATE ${UTLibDir})

         target_link_libraries(${SubTarget} PRIVATE ut.common)
         target_link_libraries(${SubTarget} PRIVATE ${Target})

         set_target_properties(${SubTarget} PROPERTIES
            VERSION ${PROJECT_VERSION}
            LIBRARY_OUTPUT_DIRECTORY ${UTLibDir}
            OUTPUT_NAME ${SubTarget}-unittest)
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
