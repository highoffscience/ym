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
set_target_properties(${Target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${UTLibDir})

set(CompileFlags -Wno-format-security)

if (YM_COV_ENABLED)
   target_link_options(${Target} PRIVATE ${CovFlags})
else()
   set(CompileFlags ${CompileFlags} -O2)
endif()

target_compile_options(${Target} PRIVATE ${CompileFlags})

unset(CompileFlags)
unset(Target)

## ym_common_subbuild
#
# @param NameOfUTSubDir -- Name of file (SUT, or UT Dir) under test.
#
function(ym_common_subbuild NameOfUTSubDir)
   set(Target ym-common-${NameOfUTSubDir})
   set(UTDirOfTarget ${CMAKE_SOURCE_DIR}/ym/common/${NameOfUTSubDir})

   add_library(${Target} SHARED)

   target_sources(${Target} PRIVATE
      ${UTDirOfTarget}/testsuite.cpp
   )

   target_include_directories(${Target} PRIVATE
      ${RootDir}/ym/common/
      ${CMAKE_SOURCE_DIR}/common/
   )

   target_link_directories(${Target} PRIVATE ${UTLibDir})
   target_link_libraries(${Target} PRIVATE ut-common)
   target_link_libraries(${Target} PRIVATE ym-common)

   set_target_properties(${Target} PROPERTIES VERSION ${PROJECT_VERSION})
   set_target_properties(${Target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${UTLibDir})

   string(TOUPPER -DYM_COMMON_UT_DBG_${NameOfUTSubDir} DbgFlags)

   set(CompileFlags ${DbgFlags} -Wno-format-security -O2)

   if (YM_COV_ENABLED)
   endif()

   target_compile_options(${Target} PRIVATE ${CompileFlags})
endfunction()
