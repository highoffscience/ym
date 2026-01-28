##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

## srcbuild-ym.common
#
# @brief Defines target to build ym.common shared library.
#
# @param Ctx_JSON -- Context object.
#
function(srcbuild-ym.common Ctx_JSON)

   set(Target ym.common)
   add_library(${Target} SHARED)

   set(Srcs
      # argparser.cpp
      # datalogger.cpp
      # fileio.cpp
      globallogger.cpp
      litelogger.cpp
      logger.cpp
      textlogger.cpp
      timer.cpp
      verbogroup.cpp
      ymassert.cpp
      ymutils.cpp)
   list(TRANSFORM Srcs PREPEND ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/)
   target_sources(${Target} PRIVATE ${Srcs})

endfunction()
