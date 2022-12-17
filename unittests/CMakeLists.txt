##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.16)

project(UnitTest VERSION 1.0.0 DESCRIPTION "Builds unittest shared libraries")

include(common.cmake)

set(UTFilePaths
   ym/common/random
)

foreach(UTFilePath IN LISTS UTFilePaths)
   set(FullUTFilePath "${RootDir}/${UTFilePath}")
   if(EXISTS  "${FullUTFilePath}/CMakeLists.txt")
      include("${FullUTFilePath}/CMakeLists.txt")
   else()
      get_filename_component(Target ${UTFilePath} NAME)
      add_library(${Target} SHARED)
      target_sources(${Target} PRIVATE
         ${UTCommonSrcs}
         ${SUTDir}/random.cpp # TODO fix this
         ${SUTUnitTestDir}/testsuite.cpp)
   endif()
endforeach()
