##
# @file    testsuite.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

## ym.common
#
# @brief TODO
#
function(ym.common)

   set(Target ${CMAKE_CURRENT_FUNCTION})
   set(SrcNames textlogger timer ymassert ymdefs ymutils)
   
   foreach(SrcName ${SrcNames})
      target_sources(${Target} PUBLIC ${CMAKE_SOURCE_DIR}/ym/common/${SrcName})
   endforeach()

   set(SubBuilds textlogger timer ymassert ymdefs ymutils)
   foreach(Build ${SubBuilds})

   endforeach()

endfunction()
