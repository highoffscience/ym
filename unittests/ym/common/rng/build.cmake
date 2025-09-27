##
# @file    build.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#
# @todo Waiting for 64-bit TestU01
#

cmake_minimum_required(VERSION 3.27)

## unitbuild-ym.common.rng
#
# @brief Defines target to build the rng unittest.
#
function(unitbuild-ym.common.rng Ctx_JSON)

   set(BaseBuild ym.common.rng)
   set(Target    ${BaseBuild}-unittest)
   set(TargetRun ${BaseBuild}-run)

   string(REPLACE "." "/" BaseBuildDir ${BaseBuild})

   add_library(${Target} SHARED)
   target_sources(${Target} PRIVATE ${YM_UnitTestDir}/${BaseBuildDir}/testsuite.cpp)
   target_link_libraries(${Target} PRIVATE ym.common-interface)

   add_custom_target(${TargetRun} DEPENDS ${Target})
   add_dependencies(${TargetRun} check-venv)
   add_dependencies(ym.common-unittests ${Target})
   add_dependencies(ym.common-run       ${TargetRun})

   add_custom_command(TARGET ${TargetRun}
      POST_BUILD
      WORKING_DIRECTORY ${YM_UnitTestDir}
      BYPRODUCTS        ${YM_UnitTestDir}/cov_build/profiles
      COMMAND ${YM_Python} run_unittest.py --unittestdir=${YM_UnitTestDir} --binarydir=${CMAKE_BINARY_DIR} --suitename=${BaseBuild} --libraryname=libym.common.so --covenabled=${YM_CovEnabled})

endfunction()
