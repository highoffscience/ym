##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

project(
   UnitTest
   VERSION     1.0.0
   DESCRIPTION "Builds unittest shared libraries")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(RootDir  ${CMAKE_SOURCE_DIR}/../../)
set(UtLibDir ${CMAKE_BINARY_DIR}/customlibs/)

set(CompileFlags
   -Werror
   -Wall
   -Wextra
   -Wno-format-security
   -O2
   -DYM_UT_DBG
   -DYM_PRINT_TO_SCREEN)

include(common/build.cmake)    # ut-common

include(ym/common/build.cmake     ) # ym-common
ym_common_subbuild(argparser      ) # ym-common-argparser
ym_common_subbuild(datalogger     ) # ym-common-datalogger
ym_common_subbuild(fileio         ) # ym-common-fileio
ym_common_subbuild(memio          ) # ym-common-memio
ym_common_subbuild(ops            ) # ym-common-ops
ym_common_subbuild(prng           ) # ym-common-prng
ym_common_subbuild(textlogger     ) # ym-common-textlogger
ym_common_subbuild(threadsafeproxy) # ym-common-threadsafeproxy
ym_common_subbuild(timer          ) # ym-common-timer
ym_common_subbuild(ymception      ) # ym-common-ymception
