##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

## ym.common.ymdefs
#
# @brief Defines targets to build and run the ymdefs testsuite.
#
function(ym.common.ymdefs Ctx_JSON)

   set(Target    ${CMAKE_CURRENT_FUNCTION}-unittest)
   set(TargetRun ${CMAKE_CURRENT_FUNCTION}-run)

   add_library(${Target} SHARED)
   add_custom_target(${TargetRun} DEPENDS ${Target})

   string(REPLACE "." "_" UTFlag ${CMAKE_CURRENT_FUNCTION}_UT)
   string(TOUPPER ${UTFlag} UTFlag)
   target_compile_definitions(${Target} ${UTFlag})

   string(REPLACE "." "/" RelPath ${CMAKE_CURRENT_FUNCTION})
   string(FIND ${CMAKE_CURRENT_FUNCTION} "." StopIdx REVERSE)
   string(SUBSTRING ${CMAKE_CURRENT_FUNCTION} 0 ${StopIdx} BaseTarget)
   
   target_sources(${Target} ${CMAKE_SOURCE_DIR}/${RelPath}/testsuite.cpp)
   target_link_libraries(${Target}    PRIVATE ${BaseTarget})
   target_link_libraries(${TargetRun} PRIVATE ${BaseTarget}-run)

endfunction()
