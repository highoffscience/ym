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
function(ym-build-integ.core Ctx_JSON)

   string(REGEX REPLACE "ym-build-" ""  BaseBuild    ${CMAKE_CURRENT_FUNCTION})
   string(      REPLACE "."         "/" BaseBuildDir ${BaseBuild})

   set(TargetAll ${BaseBuild}-tests)
   set(TargetInt ${BaseBuild}-interface)

   add_custom_target(${TargetAll})
   add_library(${TargetInt} INTERFACE)

   target_link_libraries(${TargetInt} INTERFACE YMRootIntLib)

   set(SubBuilds file_handling pointers)
   foreach(SubBuild ${SubBuilds})
      include(${YM_TestDir}/${BaseBuildDir}/${SubBuild}/build.cmake)
      cmake_language(CALL  ym-build-${BaseBuild}.${SubBuild} ${Ctx_JSON})
      add_dependencies(${TargetAll} ${BaseBuild}.${SubBuild}-tests)
   endforeach()

endfunction()
