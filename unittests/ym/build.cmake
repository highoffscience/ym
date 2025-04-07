##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

## ym
#
# @brief Defines target to build all child unittests.
#
function(ym Ctx_JSON)
   set(SubBuilds common)
   set(Target    ${CMAKE_CURRENT_FUNCTION})
   set(TargetAll ${Target}-unittests)
   set(TargetRun ${Target}-run)

   add_custom_target(${TargetAll})
   add_custom_target(${TargetRun})

   foreach(SubBuild ${SubBuilds})

      set(SubTarget    ${Target}.${SubBuild}-unittests)
      set(SubTargetRun ${Target}.${SubBuild}-run)

      include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${SubBuild}/build.cmake)
      cmake_language(CALL ${Target}.${SubBuild} Ctx_JSON)

      add_dependencies(${TargetAll} ${SubTarget})
      add_dependencies(${TargetRun} ${SubTargetRun})
   endforeach()
endfunction()
