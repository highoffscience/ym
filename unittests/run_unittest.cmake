##
# @file    run_unittest.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

# @note execute_process() will run their commands concurrently, so multiple calls
#          are necessary for sequential execution.

cmake_minimum_required(VERSION 3.27)

set(LLVM_PROFILE_FILE)

if (YM_COV_ENABLED)
   set(LLVM_PROFILE_FILE ${CMAKE_SOURCE_DIR}/covbuild/profiles/${TARGET_NAME}.profraw)
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=${LLVM_PROFILE_FILE}
   python -m unittest ${TARGET_NAME}.testsuite
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

if (YM_COV_ENABLED)
   string(REPLACE ".profraw" ".profdata" MERGED_PROFILE_FILE ${LLVM_PROFILE_FILE})
   execute_process(COMMAND llvm-profdata merge ${LLVM_PROFILE_FILE} -o ${MERGED_PROFILE_FILE})
   execute_process(COMMAND llvm-cov show ${CMAKE_SOURCE_DIR}/covbuild/customlibs/${OBJECT_NAME}
      -instr-profile=${MERGED_PROFILE_FILE} -use-color -format=html
      -output-dir=${CMAKE_SOURCE_DIR}/covbuild/profiles/${TARGET_NAME}-covprofiles)
endif()
