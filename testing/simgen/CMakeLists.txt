cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(LibClangTool LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../common/build.cmake)

srcbuild()

add_executable(libclang_tool
   simgen.cpp
)

target_compile_features(libclang_tool PRIVATE cxx_std_23)

target_include_directories(libclang_tool PRIVATE
   ../../extlibs/build/llvm-project/llvm/include
)

target_link_directories(libclang_tool PRIVATE
   ../../extlibs/build/llvm-project/llvm/lib
)

target_link_libraries(libclang_tool PRIVATE
   clang
)

add_dependencies(libclang_tool srctar)
