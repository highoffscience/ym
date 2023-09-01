##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

set(Target ym-common)

add_library(${Target} SHARED)

target_sources(${Target} PRIVATE
   ${RootDir}/ym/common/argparser.cpp
   ${RootDir}/ym/common/datalogger.cpp
   ${RootDir}/ym/common/fileio.cpp
   ${RootDir}/ym/common/logger.cpp
   ${RootDir}/ym/common/ops.cpp
   ${RootDir}/ym/common/prng.cpp
   ${RootDir}/ym/common/textlogger.cpp
   ${RootDir}/ym/common/timer.cpp
   ${RootDir}/ym/common/ymception.cpp
)

set_target_properties(${Target} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${Target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${UtLibDir})

target_compile_options(${Target} PRIVATE ${CompileFlags})

## ym_common_subbuild
#
# @param NameOfUtSubDir -- Name of file (SUT, or Ut Dir) under test.
#
function(ym_common_subbuild NameOfUtSubDir)
   set(Target        ym-common-${NameOfUtSubDir})
   set(UtDirOfTarget ${CMAKE_SOURCE_DIR}/ym/common/${NameOfUtSubDir})

   add_library(${Target} SHARED)

   target_sources(${Target} PRIVATE
      ${UtDirOfTarget}/testsuite.cpp
   )

   target_include_directories(${Target} PRIVATE
      ${RootDir}/ym/common/
      ${CMAKE_SOURCE_DIR}/common/
   )

   target_link_directories(${Target} PRIVATE ${UtLibDir})
   target_link_libraries(${Target} PRIVATE ut-common)
   target_link_libraries(${Target} PRIVATE ym-common)

   set_target_properties(${Target} PROPERTIES VERSION ${PROJECT_VERSION})
   set_target_properties(${Target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${UtLibDir})

   string(TOUPPER -DYM_COMMON_UT_DBG_${NameOfUtSubDir} DbgFlags)
   set(DbgFlags ${DbgFlags} -DYM_COMMON_UT_DBG)

   target_compile_options(${Target} PRIVATE ${CompileFlags} ${DbgFlags})
endfunction()
