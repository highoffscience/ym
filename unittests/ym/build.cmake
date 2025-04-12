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
# @param Ctx_JSON -- Context object.
#
# @note Container directory (see README for description).
#
function(ym Ctx_JSON)

   set(Target    ${CMAKE_CURRENT_FUNCTION})
   set(TargetAll ${Target}-unittests)
   set(TargetRun ${Target}-run)

   add_library(${Target} PRIVATE)
   add_custom_target(${TargetAll})
   add_custom_target(${TargetRun})

   target_link_libraries(${Target} PRIVATE YMRootInterfaceLibrary)
   target_link_libraries(${Target} PRIVATE ut.common)

   if (YM_DEBUG)
      target_compile_definitions(${Target} YM_DEBUG)
   endif()

   set(SubTargets common)
   foreach(SubTarget ${SubTargets})
      include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${SubTarget}/build.cmake)
      cmake_language(CALL ${Target}.${SubTarget} Ctx_JSON)
   endforeach()
   
endfunction()
