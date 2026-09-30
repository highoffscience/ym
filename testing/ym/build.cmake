##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

##
# @brief Defines target to build all child unit tests.
#
# @param Ctx_JSON -- Context object.
#
function(unitbuild-ym Ctx_JSON)

   string(REGEX REPLACE "^[^-]+-" ""  BaseBuild    ${CMAKE_CURRENT_FUNCTION})
   string(      REPLACE "."       "/" BaseBuildDir ${BaseBuild})

   set(TargetAll ${BaseBuild}-unittests)
   set(TargetInt ${BaseBuild}-interface)

   add_custom_target(${TargetAll})
   add_library(${TargetInt} INTERFACE)

   target_link_libraries(${TargetInt} INTERFACE YMRootIntLib)

   set(SubBuilds common)
   foreach(SubBuild ${SubBuilds})
      include(${YM_UnitTestDir}/${BaseBuildDir}/${SubBuild}/build.cmake)
      cmake_language(CALL unitbuild-${BaseBuild}.${SubBuild} ${Ctx_JSON})
   endforeach()

endfunction()
