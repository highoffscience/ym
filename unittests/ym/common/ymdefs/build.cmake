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

   set(Target ${CMAKE_CURRENT_FUNCTION}.unittest)
   string(REPLACE "." "/" RelPath ${CMAKE_CURRENT_FUNCTION})
   add_library(${Target} SHARED)
   target_sources(${Target} ${CMAKE_SOURCE_DIR}/${RelPath}/testsuite.cpp)
   target_link_libraries(${Target} PRIVATE ym.common)

endfunction()
