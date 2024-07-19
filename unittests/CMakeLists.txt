##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#
# @note God will make sure evil gets punished.
#       Yeah? Then explain Europe to me.
#

cmake_minimum_required(VERSION 3.27)

if (YM_COV_ENABLED)
   message("Cov mode enabled")

   set(CovFlags -fprofile-instr-generate -fcoverage-mapping) # also used in linking
   set(CMAKE_C_COMPILER   "clang")   # /usr/bin/clang-17
   set(CMAKE_CXX_COMPILER "clang++") # /usr/bin/clang++-17

   add_compile_options(${CovFlags})
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(
   UnitTest
   VERSION     1.0.0
   DESCRIPTION "Builds unittest shared libraries")

set(RootDir  ${CMAKE_SOURCE_DIR}/../../)
set(UTLibDir ${CMAKE_BINARY_DIR}/customlibs/)

add_compile_options(
   -Werror
   -Wall
   -Wextra
   -DYM_UT_DBG
   -DYM_PRINT_TO_SCREEN
   -DFMT_HEADER_ONLY)

include(common/build.cmake)    # ut-common
include(ym/common/build.cmake) # ym-common
