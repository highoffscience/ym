##
# @file    merge_covdata.cmake
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

separate_arguments(SUBTARGET_NAMES)
list(TRANSFORM SUBTARGET_NAMES PREPEND ${CMAKE_SOURCE_DIR}/covbuild/profiles/${TARGET_NAME}.)
list(TRANSFORM SUBTARGET_NAMES APPEND .profdata)

set(MERGED_PROFILE_FILE ${CMAKE_SOURCE_DIR}/covbuild/profiles/${TARGET_NAME}.profdata)

execute_process(COMMAND llvm-profdata merge ${SUBTARGET_NAMES} -o ${MERGED_PROFILE_FILE})
execute_process(COMMAND llvm-cov show ${CMAKE_SOURCE_DIR}/covbuild/customlibs/${OBJECT_NAME}
   -instr-profile=${MERGED_PROFILE_FILE} -use-color -format=html
   -output-dir=${CMAKE_SOURCE_DIR}/covbuild/profiles/${TARGET_NAME}-covprofiles)
