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
function(ym)
   set(SubDirs common)
   set(Target    ${CMAKE_CURRENT_FUNCTION})
   set(TargetAll ${Target}_all)
   set(TargetRun ${Target}_run)

   add_custom_target(${TargetAll})
   add_custom_target(${TargetRun})

   foreach(SubDir IN LISTS SubDirs)
      include(${CMAKE_SOURCE_DIR}/ym/${SubDir}/build.cmake)
      cmake_language(CALL ym.${SubDir})
      add_dependencies(${TargetAll} ym.${SubDir}_all)
      add_dependencies(${TargetRun} ym.${SubDir}_run)
   endforeach()

   # TODO
   # add custom command that attaches to main target that merges all cov files
endfunction()
