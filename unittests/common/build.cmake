##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

## ut.common
#
# @brief Defines target to build the unittest framework.
#
# @param Ctx_JSON -- Context object.
#
# @note Library directory (see README for description).
#
function(ut.common Ctx_JSON)

   set(Target ${CMAKE_CURRENT_FUNCTION})
   add_library(${Target} SHARED)

   set(Srcs
      datashuttle.cpp
      nameable.cpp
      testcase.cpp
      testsuitebase.cpp
      utception.cpp)
   list(TRANSFORM Srcs PREPEND ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/)
   target_sources(${Target} PRIVATE ${Srcs})

   target_include_directories(${Target} PRIVATE ${CMAKE_CURRENT_FUNCTION_LIST_DIR})

   if (YM_UT_COMMON_DEBUG)
      target_compile_definitions(${Target} PRIVATE YM_DEBUG=1)
   endif()

   target_link_libraries(${Target} YMRootIntLib)

endfunction()
