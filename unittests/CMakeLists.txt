##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.16)

project(UnitTest VERSION 1.0.0 DESCRIPTION "Builds unittest shared libraries")

include(utcommon.cmake)

set(RelativeFilePaths
   ym/common/argparser
   ym/common/ops
   ym/common/random
   ym/common/textlogger
   ym/common/timer
)

foreach(RelativeFilePath IN LISTS RelativeFilePaths)
   set(AbsoluteUTFilePath ${UnitTestDir}/${RelativeFilePath})
   if(EXISTS  ${AbsoluteUTFilePath}/CMakeLists.txt)
      include(${AbsoluteUTFilePath}/CMakeLists.txt)
   else()
      get_filename_component(Target ${RelativeFilePath} NAME)

      add_library(${Target} SHARED)

      target_sources(${Target} PRIVATE
         ${UTCommonSrcs}
         ${RootDir}/../${RelativeFilePath}.cpp
         ${AbsoluteUTFilePath}/testsuite.cpp
         ${CommonDir}/argparser.cpp
         ${CommonDir}/ops.cpp
         ${CommonDir}/logger.cpp
         ${CommonDir}/memorymanager.cpp
         ${CommonDir}/timer.cpp
         ${CommonDir}/textlogger.cpp
         ${CommonDir}/ymception.cpp)

      set_target_properties(${Target} PROPERTIES VERSION ${PROJECT_VERSION})
      set_target_properties(${Target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${RelativeFilePath}/../)

      target_compile_options(${Target} PRIVATE ${CompileFlags})

      target_include_directories(${Target} PRIVATE ${FmtDir}/include/)
      target_include_directories(${Target} PRIVATE ${CommonDir})
      target_include_directories(${Target} PRIVATE ${UnitTestDir}/common/)
      target_include_directories(${Target} PRIVATE ${RootDir}/../${RelativeFilePath})
      target_include_directories(${Target} PRIVATE ${AbsoluteUTFilePath})
   endif()
endforeach()
