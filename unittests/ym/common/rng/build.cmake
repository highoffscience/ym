##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#
# @todo Waiting for 64-bit TestU01
#

cmake_minimum_required(VERSION 3.27)

## utbuild-ym.common.rng
#
# @brief Defines target to build the rng unittest.
#
function(utbuild-ym.common.rng Ctx_JSON)

   set(BaseBuild ym.common.rng)
   set(Target    ${BaseBuild}-unittest)
   set(TargetRun ${BaseBuild}-run)

   add_library(${Target} SHARED)
   target_sources(${Target} PRIVATE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/testsuite.cpp)
   target_link_libraries(${Target} PRIVATE ym.common-interface)

   add_custom_target(${TargetRun} DEPENDS ${Target})
   add_dependencies(${TargetRun} check-venv)
   add_dependencies(ym.common-unittests ${Target})
   add_dependencies(ym.common-run       ${TargetRun})

   if (${YM_CovEnabled})
      add_custom_command(TARGET ${TargetRun}
         POST_BUILD
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
         BYPRODUCTS        ${CMAKE_SOURCE_DIR}/covbuild/profiles
         COMMAND ${YM_Python} "run_unittest.py "
            "--unittestdir=${CMAKE_SOURCE_DIR} "
            "--binarydir=${CMAKE_BINARY_DIR} "
            "--suitename=${BaseBuild} "
            "--libraryname=libym.common.so "
            "--covenabled=${YM_CovEnabled}")
   endif()

endfunction()
