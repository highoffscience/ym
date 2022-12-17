##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.16)

project(UnitTest VERSION 1.0.0 DESCRIPTION "Builds unittest shared libraries")

include(common.cmake)

set(FullFilePaths
   ym/common/random
)

foreach(FullFilePath IN LISTS FullFilePaths)
   #TODO Do I include build.cmake or CMakeLists.txt here?
   if(EXISTS "${FullFilePath}/CMakeLists.txt")
      # run that file
   else()
      # default build steps
   endif()
   message(STATUS "--> ${FullFilePath}")
endforeach()
