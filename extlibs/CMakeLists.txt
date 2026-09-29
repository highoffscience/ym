##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 4.0)

project(
   ExtLibs
   VERSION     1.0.0
   DESCRIPTION "Builds external libraries")

include(ExternalProject)

if (YM_BUILD_FMT)
   set(YM_TMP_EXTPROJ_PATH fmt)
   set(YM_TMP_EXTPROJ_NAME ${YM_TMP_EXTPROJ_PATH})
   if (NOT EXISTS ${CMAKE_SOURCE_DIR}/${YM_TMP_EXTPROJ_PATH}/CMakeLists.txt)
      message(FATAL_ERROR "${YM_TMP_EXTPROJ_NAME} submodule not fetched")
   endif()
   ExternalProject_Add(${YM_TMP_EXTPROJ_NAME}
      SOURCE_DIR ${CMAKE_SOURCE_DIR}/${YM_TMP_EXTPROJ_PATH}
      BINARY_DIR ${CMAKE_BINARY_DIR}/${YM_TMP_EXTPROJ_PATH}

      CMAKE_ARGS
         -DCMAKE_BUILD_TYPE=Release
         -DBUILD_SHARED_LIBS=TRUE
         -DFMT_TEST=OFF

      BUILD_COMMAND ${CMAKE_COMMAND} --build .
      INSTALL_COMMAND "" # no install

      # flush command outputs
      USES_TERMINAL_CONFIGURE TRUE
      USES_TERMINAL_BUILD TRUE
      USES_TERMINAL_INSTALL TRUE

      BUILD_BYPRODUCTS
         ${CMAKE_BINARY_DIR}/fmt/libfmt.so
   )
endif()

if (YM_BUILD_LLVM)
   set(YM_TMP_EXTPROJ_PATH llvm-project/llvm)
   set(YM_TMP_EXTPROJ_NAME llvm-project)
   if (NOT EXISTS ${CMAKE_SOURCE_DIR}/${YM_TMP_EXTPROJ_PATH}/CMakeLists.txt)
      message(FATAL_ERROR "${YM_TMP_EXTPROJ_NAME} submodule not fetched")
   endif()
   ExternalProject_Add(${YM_TMP_EXTPROJ_NAME}
      SOURCE_DIR ${CMAKE_SOURCE_DIR}/${YM_TMP_EXTPROJ_PATH}
      BINARY_DIR ${CMAKE_BINARY_DIR}/${YM_TMP_EXTPROJ_PATH}

      CMAKE_ARGS
         -G Ninja
         -DCMAKE_BUILD_TYPE=Release
         -DLLVM_ENABLE_PROJECTS=clang;clang-tools-extra
         -DLLVM_BUILD_TESTS=OFF
         -DLLVM_BUILD_EXAMPLES=OFF

      BUILD_COMMAND ${CMAKE_COMMAND} --build .
      INSTALL_COMMAND "" # no install

      # flush command outputs
      USES_TERMINAL_CONFIGURE TRUE
      USES_TERMINAL_BUILD TRUE
      USES_TERMINAL_INSTALL TRUE

      BUILD_BYPRODUCTS
         ${CMAKE_BINARY_DIR}/llvm-project/llvm/lib/libclang.so
   )
endif()

if (YM_BUILD_CATCH2)
   set(YM_TMP_EXTPROJ_PATH Catch2)
   set(YM_TMP_EXTPROJ_NAME ${YM_TMP_EXTPROJ_PATH})
   if (NOT EXISTS ${CMAKE_SOURCE_DIR}/${YM_TMP_EXTPROJ_PATH}/CMakeLists.txt)
      message(FATAL_ERROR "${YM_TMP_EXTPROJ_NAME} submodule not fetched")
   endif()
   ExternalProject_Add(${YM_TMP_EXTPROJ_NAME}
      SOURCE_DIR ${CMAKE_SOURCE_DIR}/${YM_TMP_EXTPROJ_PATH}
      BINARY_DIR ${CMAKE_BINARY_DIR}/${YM_TMP_EXTPROJ_PATH}

      CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        -DCATCH_BUILD_TESTING=OFF
        -DCATCH_BUILD_EXAMPLES=OFF
        -DBUILD_SHARED_LIBS=TRUE

      BUILD_COMMAND ${CMAKE_COMMAND} --build .
      INSTALL_COMMAND "" # no install

      # flush command outputs
      USES_TERMINAL_CONFIGURE TRUE
      USES_TERMINAL_BUILD TRUE
      USES_TERMINAL_INSTALL TRUE

      BUILD_BYPRODUCTS
         ${CMAKE_BINARY_DIR}/Catch2/src/libCatch2.so
   )
endif()
