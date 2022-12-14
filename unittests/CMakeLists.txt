##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.16)

project(UnitTest VERSION 1.0.0 DESCRIPTION "Builds unittest shared libraries")

include(FetchContent)

FetchContent_Declare(fmtlib
GIT_REPOSITORY https://github.com/fmtlib/fmt.git
GIT_TAG 5.3.0)
FetchContent_MakeAvailable(fmtlib)

include(ym/common/random/CMakeLists.txt)
