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

   set(TargetAll ym-unittests)
   set(TargetRun ym-run)
   set(TargetInt ym-interface)

   add_custom_target(${TargetAll})
   add_custom_target(${TargetRun})
   add_library(${TargetInt} INTERFACE)

   target_link_libraries(${TargetInt} INTERFACE YMRootIntLib)

   set(SubTargets common)
   foreach(SubTarget ${SubTargets})
      include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${SubTarget}/build.cmake)
      cmake_language(CALL utbuild-ym.${SubTarget} ${Ctx_JSON})
   endforeach()
   
endfunction()
