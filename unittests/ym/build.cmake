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

   set(Target    ${CMAKE_CURRENT_FUNCTION})
   set(TargetAll ${Target}-unittests)
   set(TargetRun ${Target}-run)
   set(TargetInt ${Target}-interface)

   add_custom_target(${TargetAll})
   add_custom_target(${TargetRun})

   add_library(${TargetInt} INTERFACE)

   set(SubTargets common)
   foreach(SubTarget ${SubTargets})
      include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${SubTarget}/build.cmake)
      cmake_language(CALL ${Target}.${SubTarget} Ctx_JSON)
   endforeach()
   
endfunction()
