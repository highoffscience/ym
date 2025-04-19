##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#
# @note God will make sure evil gets punished.
#       Yeah? Then explain Europe to me.
#

cmake_minimum_required(VERSION 3.27)

project(EXTLibs)

include(FetchContent)

set(BUILD_SHARED_LIBS TRUE)
FetchContent_Declare(
   fmt
   GIT_REPOSITORY https://github.com/fmtlib/fmt
   GIT_TAG        123913715afeb8a437e6388b4473fcc4753e1c9a) # 11.1.4
FetchContent_MakeAvailable(fmt)
