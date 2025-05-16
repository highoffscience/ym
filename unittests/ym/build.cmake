##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

## utbuild-ym
#
# @brief Defines target to build all child unittests.
#
# @param Ctx_JSON -- Context object.
#
# @note Container directory (see README for description).
#
function(utbuild-ym Ctx_JSON)

   set(BaseBuild ym)
   set(TargetAll ${BaseBuild}-unittests)
   set(TargetRun ${BaseBuild}-run)
   set(TargetInt ${BaseBuild}-interface)

   string(REPLACE "." "/" BaseBuildDir ${BaseBuild})

   add_custom_target(${TargetAll})
   add_custom_target(${TargetRun})
   add_library(${TargetInt} INTERFACE)

   target_link_libraries(${TargetInt} INTERFACE YMRootIntLib)

   set(SubBuilds common)
   foreach(SubBuild ${SubBuilds})
      include(${YM_UnitTestDir}/${BaseBuildDir}/${SubBuild}/build.cmake)
      cmake_language(CALL utbuild-${BaseBuild}.${SubBuild} ${Ctx_JSON})
   endforeach()
   
endfunction()
