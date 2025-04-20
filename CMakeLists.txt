##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

project(
   MyProject
   VERSION     1.0.0
   DESCRIPTION "Builds project")

if (${YM_COV_ENABLED})
   message("Compiling for coverage...")
endif()

if (${YM_MAKE_UNITTESTS})
   add_subdirectory(${CMAKE_SOURCE_DIR}/unittests)
endif()
