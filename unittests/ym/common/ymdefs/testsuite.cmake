##
# @file    testsuite.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

function(ym.common.ymdefs BaseTarget)

   set(Target ${CMAKE_CURRENT_FUNCTION})
   string(REGEX REPLACE "[.]" "/" Path ${Target})
   add_library(${Target} SHARED)
   target_link_libraries(${Target} PUBLIC ${BaseTarget})
   target_sources(${Target} ${Path}/testsuite.cpp)

endfunction()
