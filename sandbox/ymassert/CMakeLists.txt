##
# @file    CMakeLists.txt
# @version 1.0.0
# @author  Forrest Jablonski
#

cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-O2 -DYM_USE_FMT=1 -Wall)

project(
   ErrorSandbox
   VERSION     1.0.0
   DESCRIPTION "Builds error handling mechanism sandbbox"
)

set(Target a.out)

add_executable(${Target})

set(MY_CTX "{}")
string(JSON MY_CTX SET ${MY_CTX} my_name \"Forrest\")
message(STATUS "my ctx is ${MY_CTX}")
string(JSON MY_NAME GET ${MY_CTX} my_name)
message(STATUS "my name is ${MY_NAME}")

# set(dat "{}")
# set(fake "abc" "def" "xyz")

# # foreach(i RANGE 1)

#    string(JSON dat SET ${dat} key0 "{}")
#    # foreach(j RANGE 2)
#       # list(GET fake ${j} x)
#       string(JSON dat SET ${dat} key0 prp0 \"${x}\")
#    # endforeach()

# # endforeach()

# string(JSON KEY GET ${dat} key0)
# message(${KEY})

target_sources(${Target} PUBLIC main.cpp)

include(FetchContent)

set(BUILD_SHARED_LIBS TRUE)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        8303d140a1a11f19b982a9f664bbe59a1ccda3f4) # 11.1.2
FetchContent_MakeAvailable(fmt)

target_link_libraries(${Target} fmt::fmt)
