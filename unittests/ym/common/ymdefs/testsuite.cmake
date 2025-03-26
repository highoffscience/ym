##
# @file    testsuite.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

## ym.common.ymdefs
#
# @brief Defines targets to build and run the ymdefs testsuite.
#
function(ym.common.ymdefs BaseTarget)

   set(Target ${CMAKE_CURRENT_FUNCTION}.testsuite)
   string(REGEX REPLACE "[.]" "/" RelPath ${Target})
   add_library(${Target} SHARED)
   target_link_libraries(${Target} PUBLIC ${BaseTarget})
   target_sources(${Target} ${CMAKE_SOURCE_DIR}/${RelPath}/testsuite.cpp)

endfunction()
