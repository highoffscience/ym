##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

## ym.common
#
# @brief Defines target to build ym.common, ym.common_all, and all child unittests.
#
function(ym.common)
   set(SubBuilds argparser datalogger fileio memio ops rng textlogger threadsafeproxy timer ymception)
   set(SrcFilesPath ${YM_ProjRootDir}/ym/common/)
   set(Target ${CMAKE_CURRENT_FUNCTION})

   add_library(${Target} SHARED)

   target_sources(${Target} PRIVATE ${SrcFilesPath}/logger.cpp)

   set_target_properties(${Target} PROPERTIES VERSION ${PROJECT_VERSION})
   set_target_properties(${Target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_UTLibDir})

   target_compile_options(${Target} PRIVATE -DYM_COMMON_UT_DBG)

   if (YM_COV_ENABLED)
      target_compile_options(${Target} PRIVATE ${YM_CovFlags})
      target_link_options(   ${Target} PRIVATE ${YM_CovFlags})
   endif()

   set(TargetAll ${Target}_all)
   add_custom_target(${TargetAll})
   add_dependencies(${TargetAll} ${Target})

   foreach(SubBuild IN LISTS SubBuilds)
      if(EXISTS                           ${SrcFilesPath}/${SubBuild}.cpp)
         target_sources(${Target} PRIVATE ${SrcFilesPath}/${SubBuild}.cpp)
      endif()

      set(SubBuildUTPath ${CMAKE_SOURCE_DIR}/ym/common/${SubBuild}/)
      if(EXISTS  ${SubBuildUTPath}/build.cmake)
         include(${SubBuildUTPath}/build.cmake)
         cmake_language(CALL ym.common.${SubBuild}_unittest)
      else()
         set(SubTarget ym.common.${SubBuild}_unittest)

         add_library(${SubTarget} SHARED)

         target_sources(${SubTarget} PRIVATE ${SubBuildUTPath}/testsuite.cpp)

         target_include_directories(${SubTarget} PRIVATE
            ${YM_ProjRootDir}/ym/common/
            ${CMAKE_SOURCE_DIR}/common/)

         target_link_directories(${SubTarget} PRIVATE ${YM_UTLibDir})

         target_link_libraries(${SubTarget} PRIVATE ut.common)
         target_link_libraries(${SubTarget} PRIVATE ym.common)

         set_target_properties(${SubTarget} PROPERTIES VERSION ${PROJECT_VERSION})
         set_target_properties(${SubTarget} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${YM_UTLibDir})

         add_dependencies(${TargetAll} ${SubTarget})
      endif()
   endforeach()
endfunction()
