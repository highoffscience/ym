##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

set(Target ym.common)
set(SubBuilds argparser datalogger fileio memio ops prng textlogger threadsafeproxy timer ymception)

add_library(${Target} SHARED)

list(TRANSFORM SubBuilds PREPEND ${RootDir}/ym/common/ OUTPUT_VARIABLE Sources)
target_sources(${Target} PRIVATE ${Sources})
unset(Sources)

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

foreach(NameOfUTSubDir IN LISTS SubBuilds)

   set(SubTarget ym.common.${NameOfUTSubDir})

   add_library(${SubTarget} SHARED)

   target_sources(${SubTarget} PRIVATE ${CMAKE_SOURCE_DIR}/ym/common/testsuite.cpp)

   target_include_directories(${SubTarget} PRIVATE
      ${RootDir}/ym/common/
      ${CMAKE_SOURCE_DIR}/common/)

   target_link_directories(${SubTarget} PRIVATE ${UTLibDir})
   target_link_libraries(${SubTarget} PRIVATE ut-common)
   target_link_libraries(${SubTarget} PRIVATE ym-common)

   set_target_properties(${SubTarget} PROPERTIES VERSION ${PROJECT_VERSION})
   set_target_properties(${SubTarget} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${UTLibDir})

   string(TOUPPER -DYM_COMMON_UT_DBG_${NameOfUTSubDir} DbgFlags)

   set(CompileFlags ${DbgFlags} -Wno-format-security -O2)

   if (YM_COV_ENABLED)
   endif()

   target_compile_options(${SubTarget} PRIVATE ${CompileFlags})
endforeach()

unset(Target)
